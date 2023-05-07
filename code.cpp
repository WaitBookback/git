#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <bits/stdc++.h>
#define MAX 1024
#define int long long

using namespace std;

struct BT_Node
{
    int keynum;
    int key[MAX];
    BT_Node* parent;
    BT_Node* child[MAX];
    BT_Node()
    {
        keynum = 0;
        parent = NULL;
    }
}; //结点 结构体

int m;

BT_Node* T;

struct result
{
    BT_Node* res; //找到的结点
    int i;        //关键字在结点中的位置
    bool f;       //是否找到
};                //搜索结构体 f表示是否找到 若找到：寻找的关键字为res结点第i个

int Search(BT_Node* p, int k)
{
    int i = 1;
    while (i <= p->keynum && k > p->key[i])
        i++;
    return i;
} //在结点p中寻找关键字k并返回位置i

void SearchBTree(BT_Node* t, int k, result& res)
{
    int i = 0, found = 0;
    BT_Node* p = t, * q = NULL;
    while (p != NULL && found == 0) //在未找到关键字时遍历B_Tree
    {
        i = Search(p, k);
        if (i <= p->keynum && p->key[i] == k)
            found = 1;
        else
        {
            q = p;
            p = p->child[i - 1];
        }
    }
    if (found == 1)
    {
        res = { p, i, true };
    }
    else
    {
        res = { q, i, false };
    }
} //在树中寻找关键字k并传至引用res中

void split(BT_Node*& q, int s, BT_Node*& _p) //将q结点分裂成两个结点前一半在原节点后一半在_p结点中
{
    int i, j, n = q->keynum;
    _p = new BT_Node; //生成新节点_p
    _p->child[0] = q->child[s];
    for (i = s + 1, j = 1; i <= n; i++, j++) //将后一半移至结点_p中
    {
        _p->key[j] = q->key[i];
        _p->child[j] = q->child[i];
    }
    _p->keynum = n - s;
    _p->parent = q->parent;
    for (i = 0; i <= n - s; i++)
    {
        if (_p->child[i] != NULL)
            _p->child[i]->parent = _p;
    }
    q->keynum = s - 1;
} //分裂结点

void New_Root(BT_Node*& t, BT_Node* p, int x, BT_Node* _p)
{
    t = new BT_Node;
    t->keynum = 1;
    t->child[0] = p;
    t->child[1] = _p;
    t->key[1] = x;
    if (p)
        p->parent = t;
    if (_p)
        _p->parent = t;
    t->parent = NULL;
} //建立新根节点

void Insert(BT_Node*& q, int i, int x, BT_Node* _p)
{
    int j, n = q->keynum;
    for (j = n; j >= i; j--)
    {
        q->key[j + 1] = q->key[j];
        q->child[j + 1] = q->child[j];
    }
    q->key[i] = x;
    q->child[i] = _p;
    if (_p)
        _p->parent = q;
    q->keynum++;
} //在结点q中插入关键字x和新节点_p

void Insert_BTree(BT_Node*& t, int k, BT_Node* q, int i)
{ //在q结点中关键字位置i-1和i之间添加k
    int x, s, end = 0, NewRoot = 0;
    BT_Node* _p;
    if (!q)
    {
        New_Root(t, NULL, k, NULL);
        return;
    }
    x = k;
    _p = NULL;
    while (NewRoot == 0 && end == 0)
    {
        Insert(q, i, x, _p); // x和_p分别插到p中
        if (q->keynum < m)   //插入完成
            end = 1;
        else
        {
            s = (m + 1) / 2;
            split(q, s, _p);
            x = q->key[s];
            if (q->parent)
            {
                q = q->parent;
                i = Search(q, x); //在父结点中寻找x位置
            }
            else
                NewRoot = 1;
        }
    }
    if (NewRoot == 1) // t为空或者结点已经分裂
        New_Root(t, q, x, _p);
}

void Remove(BT_Node*& p, int i)
{
    int j, n = p->keynum;
    for (j = i; j < n; j++)
    {
        p->key[j] = p->key[j + 1];
        p->child[j] = p->child[j + 1];
    }
    p->keynum--;
}

void Successor(BT_Node*& p, int i)
{ //由后继最下层非终端结点的最小关键字代替结点中关键字key[i]
    BT_Node* child = p->child[i];
    while (child->child[0] != NULL)
        child = child->child[0];
    p->key[i] = child->key[1];
    p = child;
}
void Restore(BT_Node*& p, int i, BT_Node*& T)
{ //调整B树
    int j;
    BT_Node* _p = p->parent;
    if (!_p) //若为空根结点 则删除该节点
    {
        T = p;
        p = p->parent;
        return;
    }
    BT_Node* lc, * rc, * pr;
    int end = 0, r = 0;
    while (!end)
    {
        r = 0;
        while (_p->child[r] != p) //找到p在_p中的位置
            r++;
        if (r == 0)
        {
            r++;
            lc = NULL;
            rc = _p->child[r];
        }
        else if (r == _p->keynum)
        {
            rc = NULL;
            lc = _p->child[r - 1];
        }
        else
        {
            lc = _p->child[r - 1];
            rc = _p->child[r + 1];
        }
        if (r > 0 && lc && (lc->keynum > (m - 1) / 2))//向左兄弟借关键字
        {
            p->keynum++;
            for (j = p->keynum; j > 1; j--)//结点关键字右移
            {
                p->key[j] = p->key[j - 1];
                p->child[j] = p->child[j - 1];
            }
            p->key[1] = _p->key[r];//父亲插入到结点
            p->child[1] = p->child[0];
            p->child[0] = lc->child[lc->keynum];
            if (p->child[0])//将p中子女的父结点修改为p
            {
                p->child[0]->parent = p;
            }
            _p->key[r] = lc->key[lc->keynum];//左兄弟上移到父亲位置
            lc->keynum--;
            end = 1;
            break;
        }
        else if (_p->keynum > r && rc != NULL && (rc->keynum > (m - 1) / 2))//向右兄弟借关键字
        {
            p->keynum++;
            p->key[p->keynum] = _p->key[r];//父亲插入到结点
            p->child[p->keynum] = rc->child[0];
            if (p->child[p->keynum] != NULL)//将p中子女的父结点修改为p
            {
                p->child[p->keynum]->parent = p;
            }
            _p->key[r] = rc->key[1];//右兄弟上移到父亲位置
            rc->child[0] = rc->child[1];
            for (j = 1; j < rc->keynum; j++)//右兄弟结点关键字左移
            {
                rc->key[j] = rc->key[j + 1];
                rc->child[j] = rc->child[j + 1];
            }
            rc->keynum--;
            end = 1;
            break;
        }
        r = 0;
        while (_p->child[r] != p)//重新找到p在子树的位置
            r++;
        if (r > 0 && (_p->child[r - 1]->keynum <= (m - 1) / 2)) //与左兄弟合并
        {
            lc = _p->child[r - 1];
            p->keynum++;
            for (j = p->keynum; j > 1; j--) //将p结点关键字和指针右移1位
            {
                p->key[j] = p->key[j - 1];
                p->child[j] = p->child[j - 1];
            }
            p->key[1] = _p->key[r]; //父结点的关键字与p合并
            p->child[1] = _p->child[0]; //左兄弟右移一个指针
            _p->child[r] = lc;
            for (j = 1; j < lc->keynum + p->keynum; j++) //将结点p中关键字和指针移到p左兄弟中
            {
                lc->key[lc->keynum + j] = p->key[j];
                lc->child[lc->keynum + j] = p->child[j];
            }
            if (p->child[0])//修改p中的子女的父结点为lc
            {
                for (j = 1; j <= p->keynum; j++)
                    if (p->child[p->keynum + j])
                    {
                        p->child[p->keynum + j]->parent = lc;
                    }
            }
            lc->keynum += p->keynum;//合并后关键字的个数
            for (j = r; j < _p->keynum; j++)//将父结点中关键字和指针左移
            {
                _p->key[j] = _p->key[j + 1];
                _p->child[j] = _p->child[j + 1];
            }
            _p->keynum--;
            pr = p;
            free(pr);
            pr = NULL;
            p = lc;
        }
        else //与右兄弟合并
        {
            rc = _p->child[r + 1];
            if (r == 0)
            {
                r++;
            }
            p->keynum++;
            p->key[p->keynum] = _p->key[r];
            p->child[p->keynum] = rc->child[0];
            rc->keynum = p->keynum + rc->keynum;
            _p->child[r - 1] = rc;
            for (j = 1; j <= rc->keynum - p->keynum; j++)
            {
                rc->key[p->keynum + j] = rc->key[j];
                rc->child[p->keynum + j] = rc->child[j];
            }
            for (j = r; j < _p->keynum; j++)
            {
                _p->key[j] = _p->key[j + 1];
                _p->child[j] = _p->child[j + 1];
            }
            _p->keynum--;
            pr = p;
            free(pr);
            pr = NULL;
            p = rc;
        }
        if (p->parent->keynum >= (m - 1) / 2 || (_p == NULL && p->parent->keynum > 0))
            end = 1;
        else if (_p == NULL)
        {
            pr = T;
            T = p;
            free(pr);
            pr = NULL;
            end = 1;
        }
        p = p->parent;
    }
}

void Delete_BTree(BT_Node*& p, int i, BT_Node*& T)
{ //删除p结点的第i个关键字
    if (p->child[i] != NULL)
    {                          //不是最下层结点时
        Successor(p, i);       //在子树中找到最下层最小关键字并替代
        Delete_BTree(p, 1, T); //删除最下层最小关键字
    }
    else
    {
        Remove(p, i);
        if (p->keynum < (m - 1) / 2) //删除后关键字少于（m-1）/2
            Restore(p, i, T);        //调整
    }
}
// BFS
void show(BT_Node*& p)
{ //打印B树
    if (p == NULL)
    {
        cout << "B-tree element printing ends" << endl;
        return;
    }
    bool end = false;
    cout << "[";
    for (int i = 1; i <= p->keynum; i++)
    {
        if (i != 1)
            cout << " ";
        cout << p->key[i];
    }
    cout << "]";
    for (int i = 0; i <= p->keynum; i++)
    {
        if (p->child[i] != NULL)
        {
            if (i == 0)
                cout << "<";
            else
                cout << ",";
            show(p->child[i]);
            end = true;
        }
    }
    if (end)
        cout << ">";
}

void show_Btree2(BT_Node*& p, int deep)
{ //打印树形B树
    if (p == NULL)
    {
        return;
    }
    int i;
    for (i = 0; i < p->keynum; i++)
    {
        show_Btree2(p->child[i], deep + 1);
        for (int i = 0; i < deep; i++)
        {
            printf("\t");
        }
        printf("%d\n", p->key[i + 1]);
    }
    show_Btree2(p->child[i], deep + 1);
}

void Destory(BT_Node*& t)
{ //删除并释放上一棵B树
    int i = 0;
    if (t != NULL)
    {
        while (i < t->keynum)
        {
            Destory(t->child[i]);
            free(t->child[i]);
            i++;
        }
    }
    free(t);
    t = NULL;
}

void Creat_BTree(int i)
{ //创建新B树
    if (i == 0)
        system("cls");

    cout << "Enter the order of creation" << endl;
    cin >> m;
    T = new BT_Node;
    T->keynum = 0;
    for (int i = 0; i < m; i++)
        T->child[i] = NULL;
    cout << "The new B-tree was successful" << endl;
    cout << "Enter the initial element in the b-tree" << endl;
    cout << "Ends with -1" << endl;
    queue<int> temp;
    while (1)
    {
        int a;
        cin >> a;
        if (a == -1)
            break;
        temp.push(a);
    }
    while (!temp.empty())
    {
        result p;
        SearchBTree(T, temp.front(), p);
        if (!p.f)
        {
            Insert_BTree(T, temp.front(), p.res, p.i);
            // cout << "Added successfully" << endl;
            // show(T);
            // cout << endl;
        }
        else
            cout << "The element(" << temp.front() << ") already exists in the b-tree" << endl;
        temp.pop();
    }
    cout << "Added successfully" << endl;
    // show(T);
    cout << endl;
}
void Insert_key()
{ //插入关键字
    system("cls");
    cout << "Enter the number you want to insert" << endl;
    cout << "Ends with -1" << endl;
    queue<int> temp;
    while (1)
    {
        int a;
        cin >> a;
        if (a == -1)
            break;
        temp.push(a);
    }
    while (!temp.empty())
    {
        result p;
        SearchBTree(T, temp.front(), p);
        if (!p.f)
        {
            Insert_BTree(T, temp.front(), p.res, p.i);
        }
        else
            cout << "The element(" << temp.front() << ") already exists in the b-tree" << endl;
        temp.pop();
    }
    cout << "The insertion succeeded" << endl;
    show(T);

    cout << endl;
}

void find_key()
{ //寻找关键字
    system("cls");

    cout << "Enter the element you want to look up" << endl;
    int temp;
    cin >> temp;
    result p;
    SearchBTree(T, temp, p);
    if (p.f)
    {
        cout << "The element is found in the b-tree" << endl;
        return;
    }
    cout << "The element was not found in the b-tree" << endl;
}

void Delete_key()
{ //删除关键字
    system("cls");

    cout << "Enter the element you want to delete" << endl;
    int t;
    cin >> t;
    result p;
    SearchBTree(T, t, p);
    if (p.f)
    {
        Delete_BTree(p.res, p.i, T);
        cout << "Delete successful" << endl;
        // show(T);
        cout << endl;
    }
    else
    {
        cout << "The deletion failed because the element was not found in the B-tree" << endl;
    }
}
void Menu()
{ //显示菜单
    puts("---------------------------------------------------------------------");
    puts("Select menu");
    puts("1:Creat a new 3-B-tree");
    puts("2:Insert element");
    puts("3:Find element");
    puts("4:Delete element");
    puts("5:Print B-tree");
    puts("6:Exit");
}

signed main()
{
    int a;
    // freopen("in.txt","r",stdin);//测试
    // freopen("out.txt","w",stdout);
    while (1)
    {
        system("cls");
        system("color f0");
        cout << "Elements currently exist in the B-tree:" << endl
            << endl;
        show(T);
        cout << endl;
        Menu();
        cin >> a;
        switch (a)
        {
        case 1:
        {
            system("color f0");

            Destory(T);
            Creat_BTree(0);
            show(T);
            cout << endl;
            cout << "Created successfully" << endl;
            Sleep(3000);
            break;
        }
        case 2:
        {
            system("color f0");

            Insert_key();
            Sleep(1000);

            break;
        }
        case 3:
        {
            system("color f0");

            find_key();
            Sleep(1000);

            break;
        }
        case 4:
        {
            system("color f0");

            Delete_key();
            Sleep(1000);
            show(T);

            Sleep(1000);
            break;
        }
        case 5:
        {
            system("cls");
            system("color f0");

            show(T);
            cout << endl;
            show_Btree2(T, 0);
            cout << endl;
            Sleep(50000000);

            break;
        }
        case 6:
        {
            return 0;
        }
        default:
        {
            cout << "Input error" << endl
                << "Re-enter" << endl;
        }
        }
    }

    return 0;
}