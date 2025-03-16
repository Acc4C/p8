#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "tree_transform.h"

int tree_must_be_transformed_unary(TreeNode **node)
{

    return ((*node) != NULL) && (token_is_unary_operator(&(*node)->token)) &&
           ((token_is_integer_value(&(*node)->right->token)) ||
            (token_is_double_value(&(*node)->right->token)));
}

int tree_must_be_transformed_multiplication(TreeNode **node)
{
    return ((*node) != NULL) && (token_is_operator(&(*node)->token))
           && (token_is_multiplication(&(*node)->token))
           && (token_is_integer_value(&(*node)->right->token));

}

void transform_unary(TreeNode **node)
{
    TreeNode *temp_node = (TreeNode *) malloc(sizeof(TreeNode));
    if (token_is_minus(&(*node)->token)) {
        if (token_is_integer_value(&(*node)->right->token)) {
            temp_node->token.type = INTEGER_VALUE;
            temp_node->token.data.int_value = -(*node)->right->token.data.int_value;
        } else {
            temp_node->token.type = DOUBLE_VALUE;
            temp_node->token.data.double_value = -(*node)->right->token.data.double_value;
        }
    } else if (token_is_plus(&(*node)->token)) {
        if (token_is_integer_value(&(*node)->right->token)) {
            temp_node->token.type = INTEGER_VALUE;
            temp_node->token.data.int_value = (*node)->right->token.data.int_value;
        } else {
            temp_node->token.type = DOUBLE_VALUE;
            temp_node->token.data.double_value = (*node)->right->token.data.double_value;
        }
    }
    temp_node->left = NULL;
    temp_node->right = NULL;
    tree_delete(node);
    *node = temp_node;
}


void transform_multiplication(TreeNode **node){
    char sign = '+';
    int iter_count = (*node)->right->token.data.int_value;
    TreeNode *temp_node;
    if (iter_count == 0){
        temp_node = (TreeNode *) calloc(1, sizeof(TreeNode));
        temp_node->token.type = INTEGER_VALUE;
        temp_node->token.data.int_value = 0;
        tree_delete(node);
        *node = temp_node;
        return;
    }
    TreeNode *pretemp, *term = (TreeNode *) calloc(1, sizeof(TreeNode));
    if (iter_count < 0){
        sign = '-';
        iter_count = -iter_count;
        if (token_is_unary_operator(&(*node)->left->token)){
            sign = '+';
            if (term->token.data.operator == '+'){
                (*node)->left->token.data.operator = '-';
            } else{
                (*node)->left->token.data.operator = '+';
            }
            memcpy(term, (*node)->left, sizeof(TreeNode));
        } else if (token_is_integer_value(&(*node)->left->token) || token_is_double_value(&(*node)->left->token)){
            sign = '+';
            if (token_is_integer_value(&(*node)->left->token)){
                (*node)->left->token.data.int_value = -((*node)->left->token.data.int_value);
            } else{
                (*node)->left->token.data.double_value = -((*node)->left->token.data.double_value);
            }
            memcpy(term, (*node)->left, sizeof(TreeNode));
        } else{
            term->token.type = UNARY_OPERATOR;
            term->token.data.operator = '-';
            term->right = (TreeNode *) malloc(sizeof(TreeNode));
            memcpy(term->right, (*node)->left, sizeof(TreeNode));
        }
    } else{
        memcpy(term, (*node)->left, sizeof(TreeNode));
    }
    for (int i = 1; i < iter_count; i++){
        pretemp = term;
        term = (TreeNode *) calloc(1, sizeof(TreeNode));
        term->token.type = BINARY_OPERATOR;
        term->token.data.operator = sign;
        term->left = pretemp;
        term->right = (TreeNode *) malloc(sizeof(TreeNode));
        memcpy(term->right, (*node)->left, sizeof(TreeNode));
    }
    node_delete(node);
    *node = term;
}

void tree_transform(TreeNode **node)
{
    if ((*node) != NULL) {
        tree_transform(&((*node)->left));
        tree_transform(&((*node)->right));

        if (tree_must_be_transformed_unary(node)) {
            transform_unary(node);
        }

        if (tree_must_be_transformed_multiplication(node)) {
            transform_multiplication(node);
        }
    }
}
