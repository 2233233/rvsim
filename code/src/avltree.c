//edit form https://blog.csdn.net/weixin_54186646/article/details/124557830

#include "header/avltree.h"
int get_height(Node* node)
{
    return HEIGHT(node);
}

/*插入左孩子的左子树-右旋*/
//传入参数为最小失衡结点tree，对tree进行右旋
Node* left_left(Node* tree)
{
    //结点调整
    Node* k = tree->left;//保存tree的左孩子，k将是最终的父节点
    tree->left = k->right;//将k的右孩子接到tree的左子树
    k->right = tree;//tree作为k的右子树

    //高度调整（这里指深度：用左右子树来判断）
    k->height = MAX(get_height(k->left), get_height(k->right)) + 1;
    tree->height = MAX(get_height(tree->left), get_height(tree->right)) + 1;
    return k;
}

/*插入右孩子的右子树-左旋*/
//传入参数为最小失衡结点tree，对tree进行左旋
Node* right_right(Node* tree)
{
    //结点调整
    Node* k = tree->right;
    tree->right = k->left;
    k->left = tree;

    //高度调整
    k->height = MAX(get_height(k->left), get_height(k->right)) + 1;
    tree->height = MAX(get_height(tree->left), get_height(tree->right)) + 1;
    return k;
}

/*插入左孩子的右子树-先左旋再右旋*/
//对tree->left左旋(left_left)，对tree右旋(right_right)
Node* left_right(Node* tree)
{
    tree->left = right_right(tree->left);
    tree = left_left(tree);
    return tree;
}

/*插入右孩子的左子树-先右旋再左旋*/
//对tree->right右旋(left_left),对tree左旋
Node* right_left(Node* tree)
{
    tree->right = left_left(tree->right);
    tree = right_right(tree);
    return tree;
}

/*创建一棵树，根结点为node*/
Node* create(uint32_t key)
{
    Node* node = (Node*)malloc(sizeof(Node));
    //此处可判断是否创建成功，我省略了
    node->data = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 0;
    return node;
}

/*往根节点为tree的树中插入一个值key*/
//插入位置同二叉排序树的逻辑，大于向右找位置，小于向左找位置
Node* Insert(Node* tree, uint32_t key)
{
    //如果为空，就创建一棵树
    if (tree == NULL)
    {
        Node* node = create(key);
        tree = node;
    }
        //向左子树插入
    else if (key < tree->data)
    {
        //递归寻找插入位置
        tree->left = Insert(tree->left, key);
        //判断是否失衡
        if (get_height(tree->left) - get_height(tree->right) == 2)
        {
            //判断插入位置在左孩子的左子树还是右子树
            if (key < tree->left->data)
                tree = left_left(tree);
            else
                tree = left_right(tree);
        }
    }
        //向右子树插入
    else if (key > tree->data)
    {
        tree->right = Insert(tree->right, key);
        if (get_height(tree->right) - get_height(tree->left) == 2)
        {
            if (key > tree->right->data)
                tree = right_right(tree);
            else
                tree = right_left(tree);
        }
    }
    //重新调整二叉树深度
    tree->height = MAX(get_height(tree->left), get_height(tree->right)) + 1;
    return tree;
}

/*查找结点*/
Node* search(Node* tree, uint32_t key)
{
    if (tree == NULL || tree->data == key)
        return tree;
    else if (key < tree->data)
        search(tree->left, key);
    else
        search(tree->right, key);
}
/*找到替换结点-左子树的最右边*/
Node* mininum(Node* tree)
{
    if (tree == NULL)
        return NULL;
    while (tree->left)
        tree = tree->left;
    return tree;
}
/*删除结点*/
Node* del(Node* tree, uint32_t key)
{
    //定位到要删除结点
    Node* node = search(tree, key);
    if (tree == NULL || node == NULL)
    {
        return tree;
    }
    //若删除结点在左子树
    if (key < tree->data)
    {
        //递归找到要删除结点
        tree->left = del(tree->left, key);
        //删除后要检查平衡
        if (get_height(tree->right) - get_height(tree->left) == 2)
        {
            if (key < tree->right->data)
                tree = right_left(tree);
            else
                tree = right_right(tree);
        }
    }
        //若删除结点在右子树
    else if (key > tree->data)
    {
        tree->right = del(tree->right, key);
        if (get_height(tree->left) - get_height(tree->right) == 2)
        {
            if (key < tree->left->data)
                tree = left_left(tree);
            else
                tree = left_right(tree);
        }
    }
        //此时就是要删除结点,//待删除结点有左右孩子-同二叉排序树
    else if (tree->left != NULL && tree->right != NULL)
    {
        Node* min_node = mininum(tree->right);
        tree->data = min_node->data;
        tree->right = del(tree->right, min_node->data);
    }
        //只有一个孩子或者没有孩子
    else
        tree = tree->left ? tree->left : tree->right;

    if (tree)
        tree->height = MAX(get_height(tree->left), get_height(tree->right)) + 1;
    return tree;
}

void free_tree(Node* tree){
    if (tree)
    {
        free_tree(tree->left);
        free_tree(tree->right);
        free(tree);
    }
}

///*前序遍历*/
//void pre_order(Node* tree)
//{
//    if (tree)
//    {
//        printf("%d ", tree->data);
//        pre_order(tree->left);
//        pre_order(tree->right);
//    }
//}
//
///*中序遍历*/
//void in_order(Node* tree)
//{
//    if (tree)
//    {
//        in_order(tree->left);
//        printf("%d ", tree->data);
//        in_order(tree->right);
//    }
//}


//
//int main()
//{
//    //第一种情况-左孩子的左子树
//    Node* tree1 = NULL;
//    int a1[] = { 13, 8, 15, 3, 10};
//    int l1 = sizeof(a1) / sizeof(int);
//    for (int i = 0; i < l1; i++)
//    {
//        tree1 = Insert(tree1, a1[i]);
//    }
//    printf("第一种情况-左孩子的左子树\n");
//    printf("前序遍历:");
//    pre_order(tree1);
//    printf("\n");
//    printf("中序遍历:");
//    in_order(tree1);
//    printf("\n");
//    printf("根结点的深度为:%d\n\n",tree1->height);
//
//    printf("插入1\n");
//    tree1 = Insert(tree1, 1);
//    printf("前序遍历:");
//    pre_order(tree1);
//    printf("\n");
//    printf("中序遍历:");
//    in_order(tree1);
//    printf("\n");
//    printf("根结点的深度为:%d\n\n", tree1->height);
//
//    printf("删除结点8\n");
//    tree1 = del(tree1, 8);
//    printf("前序遍历:");
//    pre_order(tree1);
//    printf("\n");
//    printf("中序遍历:");
//    in_order(tree1);
//    printf("\n");
//    printf("根结点的深度为:%d\n\n", tree1->height);
//
//    printf("删除结点13\n");
//    tree1 = del(tree1, 13);
//    printf("前序遍历:");
//    pre_order(tree1);
//    printf("\n");
//    printf("中序遍历:");
//    in_order(tree1);
//    printf("\n");
//    printf("根结点的深度为:%d\n\n", tree1->height);
//
//    printf("删除结点3\n");
//    tree1 = del(tree1, 3);
//    printf("前序遍历:");
//    pre_order(tree1);
//    printf("\n");
//    printf("中序遍历:");
//    in_order(tree1);
//    printf("\n");
//    printf("根结点的深度为:%d\n\n", tree1->height);
//    //第二种情况-右孩子的右子树
//    Node* tree2 = NULL;
//    int a2[] = { 13, 8, 15, 14, 16  };
//    int l2 = sizeof(a2) / sizeof(int);
//    for (int i = 0; i < l2; i++)
//    {
//        tree2 = Insert(tree2, a2[i]);
//    }
//    printf("第二种情况-右孩子的右子树\n");
//    printf("前序遍历:");
//    pre_order(tree2);
//    printf("\n");
//    printf("中序遍历:");
//    in_order(tree2);
//    printf("\n");
//    printf("根结点的深度为:%d\n\n", tree2->height);
//
//    tree2 = Insert(tree2, 20);
//    printf("插入20\n");
//    printf("前序遍历:");
//    pre_order(tree2);
//    printf("\n");
//    printf("中序遍历:");
//    in_order(tree2);
//    printf("\n");
//    printf("根结点的深度为:%d\n\n", tree2->height);
//
//    //第三种情况-左孩子的右子树
//    Node* tree3 = NULL;
//    int a3[] = { 13, 8, 15, 3, 10  };
//    int l3 = sizeof(a3) / sizeof(int);
//    for (int i = 0; i < l3; i++)
//    {
//        tree3 = Insert(tree3, a3[i]);
//    }
//    printf("第三种情况-左孩子的右子树\n");
//    printf("前序遍历:");
//    pre_order(tree3);
//    printf("\n");
//    printf("中序遍历:");
//    in_order(tree3);
//    printf("\n");
//    printf("根结点的深度为:%d\n\n", tree3->height);
//
//    tree3 = Insert(tree3, 9);
//    printf("插入9\n");
//    printf("前序遍历:");
//    pre_order(tree3);
//    printf("\n");
//    printf("中序遍历:");
//    in_order(tree3);
//    printf("\n");
//    printf("根结点的深度为:%d\n\n", tree3->height);
//
//
//    //第四种情况 - 右孩子的左子树
//    Node* tree4 = NULL;
//    int a4[] = { 13, 8, 18, 15, 20  };
//    int l4 = sizeof(a4) / sizeof(int);
//    for (int i = 0; i < l4; i++)
//    {
//        tree4 = Insert(tree4, a4[i]);
//    }
//    printf("第四种情况-右孩子的左子树\n");
//    printf("前序遍历:");
//    pre_order(tree4);
//    printf("\n");
//    printf("中序遍历:");
//    in_order(tree4);
//    printf("\n");
//    printf("根结点的深度为:%d\n\n", tree4->height);
//
//    tree4 = Insert(tree4, 14);
//    printf("插入14\n");
//    printf("前序遍历:");
//    pre_order(tree4);
//    printf("\n");
//    printf("中序遍历:");
//    in_order(tree4);
//    printf("\n");
//    printf("根结点的深度为:%d\n\n", tree4->height);
//
//}