#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

int size = 512, N = 8, cnt = 0;
double pi = 3.14159265358979323846;

struct Node
{
    int codeLength;
    double freq, value;
    char *codeWord;
    int codeWordLength;

    struct Node* parent;
    struct Node* left, * right, * next, * prev;
};

typedef struct Node Node;

double avgLength(Node* n)
{
    return (double)(strlen(n->codeWord) * n->freq);
}

double log2Psi(Node* n)
{
    return -log2(n->freq);
}

double Entropy(Node* n)
{
    return -log2(n->freq) * n->freq;
}

void CodewordAdd(char c, Node* n)
{
    n->codeWord[n->codeWordLength] = c;
    n->codeWord[n->codeWordLength + 1] = '\0';
    n->codeWordLength++;
}

Node *CreateNode(int value, double freq)
{
    Node* n = (Node*)malloc(sizeof(Node));
    n->freq = freq;
    n->value = value;
    n->parent = n->left = n->right = n->next = n->prev = NULL;
    n->codeWordLength = 0;
    n->codeWord = (char*)malloc(sizeof(char) * 300);

    return n;
}

int CompareNodes(Node *a, Node *b)
{
    return a->freq - b->freq;
}

// head 변경 시, 바뀐 Head 주소를 반환
// 삽입 시 부터 내림차순으로 삽입
Node* InsertNode(Node *head, Node *n)
{
    Node* cur = head;

    if (head == NULL)
        return n;

    // head와의 변경을 상정
    if (head->freq < n->freq)
    {
        n->next = head;
        head->prev = n;
        return n;
    }
    // head 뿐인 경우를 상정
    else if (head != NULL && head->next == NULL)
    {
        head->next = n;
        n->prev = head;

        return head;
    }

    while (1)
    {
        //printf("inserting.. cur = %.5lf\n", cur->value);
        
        if (cur->freq < n->freq)
        {
            if (cur->prev != NULL)
                cur->prev->next = n;
            n->prev = cur->prev;

            cur->prev = n;
            n->next = cur;
            return head;
        }

        cur = cur->next;

        if (cur->next == NULL)
        {
            cur->next = n;
            n->prev = cur;
            return head;
        }
    }
    return head;
}

Node* RemoveNode(Node* listHead, Node* target)
{
    if (target == listHead)
    {
        if (target->next != NULL)
            target->next->prev = NULL;

        Node* n = target->next;

        target->next = NULL;
        target->prev = NULL;

        return n;
    }
    else
    {
        Node* cur = listHead;

        Node* prev = target->prev;
        Node* next = target->next;

        if (prev != NULL)
            prev->next = next;
        if (next != NULL)
            next->prev = prev;

        target->next = target->prev = NULL;

        return listHead;
    }
}

// 없다면 NULL, 있다면 해당 노드를 반환
Node* Contains(Node* head, int value)
{
    Node* cur = head;

    if (cur == NULL)
        return 0;

    while (cur != NULL)
    {
        if (cur->value == value)
            return cur;

        cur = cur->next;
    }

    return 0;
}


unsigned char** ReadImage()
{
    FILE* origin;

    unsigned char** in;
    int i;

    in = (unsigned char**)malloc(sizeof(unsigned char*) * size);

    for (i = 0; i < size; i++)
    {
        in[i] = (unsigned char*)malloc(sizeof(unsigned char) * size);
    }

    // 파일 읽기
    origin = fopen("lena.img", "rb");
    if (origin == NULL) {
        perror("Error opening file");
    }

    for (i = 0; i < size; i++)
    {
        fread(in[i], sizeof(unsigned char), size, origin);
    }
    fclose(origin);


    return in;
}


double** GetDPCM(unsigned char** in)
{
    double** dpcm = (double**)malloc(sizeof(double*) * size);
    double val = in[0][0];

    //init
    for (int i = 0; i < size; i++)
        dpcm[i] = (double*)malloc(sizeof(double) * size);
    //

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (i + j == 0)
            {
                dpcm[i][j] = (double)in[i][j];
                continue;
            }
            double result = (double)in[i][j] - val;
            dpcm[i][j] = (double)in[i][j] - val;
            val = (double)in[i][j];
        }
    }

    return dpcm;
}

Node** ProcessHuffman(double **dpcm)
{
    Node** leafList = NULL; // 초기 노드들 의미
    Node* list = NULL; // 삽입 과정을 거쳐서 얻게 되는 리스트, 연산을 위해 중간 중간 삽입 삭제가 일어날 것임.

    // Create Nodes, without considering freq
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            Node* contains = Contains(list, dpcm[i][j]);
            if (contains != NULL)
            {
                // counting
                contains->freq+=1.0;
                continue;
            }
            else
            {
                Node* n = CreateNode(dpcm[i][j], 1);
                list = InsertNode(list, n);
                cnt++;
            }
        }
    }
    printf("cnt = %d\n", cnt);


    //copy initials
    leafList = (Node**)malloc(sizeof(Node*) * cnt);


    int i = 0;

    // Decide Freq
    Node* n = list;
    while (n != NULL)
    {
        n->freq /= (double)(size * size);

        leafList[i] = n;
        i++;

        n = n->next;
    }

    // sort order by freq in list
    list = NULL;
    for (int i = 0; i < cnt; i++)
    {
        leafList[i]->prev = leafList[i]->next = NULL;
    }

    for (int i = 0; i < cnt; i++)
    {
        list = InsertNode(list, leafList[i]);
    }

    n = list;


    for (int i = 0; i < cnt; i++) // 오름차순으로 다시 leafList에 저장
    {
        leafList[i] = n;
        n = n->next;
    }

    // Make Tree
    for (int i = 0; i < cnt - 1; i++) // huffman 계산은 값의 개수 - 1 만큼 수행된다.
    {
        n = list;
        Node* tail0, * tail1; // tail0 = 최소, tail1 = 최소보다는 큰 값들 중 최소
        while (n->next != NULL)
        {
            n = n->next;
        }
        tail0 = n;
        tail1 = n->prev;

        double sum = tail0->freq + tail1->freq;
        if (sum >= 1.0)
            break;

        Node* newNode = CreateNode(-1, tail0->freq + tail1->freq); // value는 상관없고 freq만 있으면 됨
        
        tail0->parent = tail1->parent = newNode;

        newNode->left = tail0;
        newNode->right = tail1;

        CodewordAdd('0', tail0);
        CodewordAdd('1', tail1);

        list = InsertNode(list, newNode);

        list = RemoveNode(list, tail0);
        list = RemoveNode(list, tail1);
    }

    for (int i = 0; i < cnt; i++)
    {
        n = leafList[i];
        Node* cur = n->parent;
        while (1)
        {
            CodewordAdd(cur->codeWord[0], n);

            cur = cur->parent;

            if (cur == NULL)
                break;
        }
    }

    return leafList;
}

void stringReverse(char* str)
{
    int len = strlen(str);

    for (int i = 0, j = len - 1; i < j; i++, j--)
    {
        char ch = str[i];
        str[i] = str[j];
        str[j] = ch;
    }
}


int main()
{
    FILE* result;
    unsigned char** in, **dpcm;
    Node** huffman;
    double** dpcmRealVals;
    int i;

    dpcm = (unsigned char**)malloc(sizeof(unsigned char*) * size);
    for (int i = 0; i < size; i++)
        dpcm[i] = (unsigned char*)malloc(sizeof(unsigned char) * size);


    in = ReadImage();

    dpcmRealVals = GetDPCM(in);

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            double val = dpcmRealVals[i][j];
            if (val > 255)
                val = 255;
            else if (val < 0)
                val = 0;

            dpcm[i][j] = (unsigned char)val;
        }
    }


    // 파일 쓰기
    result = fopen("DPCM.img", "wb");
    if (result == NULL) {
        perror("Error opening file");
        return 1;
    }
    for (i = 0; i < size; i++)
    {
        fwrite(dpcm[i], sizeof(unsigned char), size, result);
    }
    fclose(result);


    huffman = ProcessHuffman(dpcmRealVals);

    // add 과정이 reverse 된 형태일 것이기에 뒤집기
    for (int i = 0; i < cnt; i++)
    {
        stringReverse(huffman[i]->codeWord);
    }

    // huffman 저장 과정
    double entropySum = 0;
    double avgLenSum = 0;
    double PsiSum = 0;
    result = fopen("Huffman table.txt", "w");
    for (int i = 0; i < cnt; i++)
    {
        double entropy = Entropy(huffman[i]);
        double avgLen = avgLength(huffman[i]);
        fprintf(result, "s %d | P(Si): %.10lf | -log2(P(Si)): %.10lf | Entropy: %.10lf  | Codeword: %s | CodeLength: %d | AVG.Length: %.10lf \n",
            i, huffman[i]->freq, log2Psi(huffman[i]), entropy, huffman[i]->codeWord, strlen(huffman[i]->codeWord), avgLen);
        PsiSum += huffman[i]->freq;
        entropySum += entropy;
        avgLenSum += avgLen;
    }

    printf("freq sum = %lf\n", PsiSum);
    printf("avgLenSum = %lf\n", avgLenSum);
    printf("entropySum = %lf\n", entropySum);

    fprintf(result, "total entropy = %.10lf | total AVG.Length = %.10lf\n", entropySum, avgLenSum);
    
    for (i = 0; i < cnt; i++)
        free(huffman[i]);
    free(huffman);

    // 메모리 해제
    for (i = 0; i < size; i++)
    {
        free(in[i]);
        free(dpcm[i]);
    }
    free(in);
    free(dpcm);

    return 0;
}
