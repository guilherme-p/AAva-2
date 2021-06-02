#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 6

/* ---------------- */
/* ------misc------ */
/* ---------------- */

void *safe_malloc(int size) {
    void *ptr = (void *) malloc(size);

    if (ptr == NULL) {
        perror("malloc error");
        exit(1);
    }

    return ptr;
}

void safe_scanf(char *format, void *ptr) {
    if (scanf(format, ptr) <= 0) {
        perror("scanf error");
        exit(1);
    }
}

void free_strings(char **strings, int size) {
    int i;

    for (i = 0; i < size; i++) {
        free(strings[i]);
    }

    free(strings);
}

long int max(long int s1, long int s2) {
    return (s1 > s2) ? s1 : s2;
}

int get_char_index(char c) {
    if (c == 'A') {
        return 0;
    } else if (c == 'C') {
        return 1;
    } else if (c == 'G') {
        return 2;
    } else if (c == 'T') {
        return 3;
    } else if (c == '$') {
        return 4;
    } else {
        return 5;
    }
}

/* --------------- */
/* -----input----- */
/* --------------- */

char *read_line() {
    int str_size;
    char *buffer;

    safe_scanf("%d", &str_size);

    buffer = (char *) safe_malloc((str_size + 1 + 1) * sizeof(char));       /* \n + \0 */
    safe_scanf("%s", buffer);

    buffer[str_size] = '\0';                                                /* remove \n */
    return buffer;
}

char *get_generalized_string(char **strings, int n, int total_size) {
    int i, j, k = 0;

    char *generalized_string = (char *) safe_malloc(sizeof(char) * (total_size + 1));

    for (i = 0; i < n; i++) {
        for (j = 0; strings[i][j] != '\0'; j++) {
            generalized_string[k++] = strings[i][j];
        }

        generalized_string[k++] = '$';
    }

    generalized_string[k++] = '\0';
    return generalized_string;
}


/* ------------------- */
/* -----ukkonen's----- */
/* ------------------- */

struct suffix_tree_node {
    struct suffix_tree_node *children[ALPHABET_SIZE];
 
    
    struct suffix_tree_node *suffix_link;
 
    int start;
    int *end;
 
    int suffix_index;

    unsigned long *bit_array;
};
 
typedef struct suffix_tree_node node;
 
char *text = NULL;
node *root = NULL; 
 
node *last_new_node = NULL;
node *active_node = NULL;
 
int active_edge = -1;
int active_length = 0;
 
int remaining_suffix_count = 0;
int leaf_end = -1;
int *root_end = NULL;
int *split_end = NULL;

int *cumulative_sizes = NULL;

int total_size = 0;
int bit_array_size, N;

void print_bit_array(unsigned long *bit_array) {
    int i, j;
    unsigned long temp;

    printf("bit array (reverse order): ");
    
    for (i = 0; i < bit_array_size; i++) {
        temp = bit_array[i];
        j = 0;

        while (temp) {
            printf("%ld", temp & 1);
            temp >>= 1;

            j++;
        }

        while (j < 64) {
            printf("%c", '0');
            j++;
        }

        printf("temp: %lu, i: %d, size: %d\n", temp, i, bit_array_size);
    }

    printf("\n");
}

void set_bit(unsigned long *bit_array, int pos) {
    bit_array[pos/64] |= 1UL << (pos % 64);
}

void merge_bit_arrays(unsigned long *a1, unsigned long *a2) {
    int i;

    for (i = 0; i < bit_array_size; i++) {
        a1[i] |= a2[i];
    }
}

int count_bits(unsigned long *bit_array) {
    int i, count = 0;
    unsigned long temp;

    for (i = 0; i < bit_array_size; i++) {
        temp = bit_array[i];

        while (temp) {
            count += temp & 1;
            temp >>= 1;
        }
    }

    return count;
}

int get_k(int suffix_index) {
    int i;
    
    for (i = 0; i < N; i++) {
        if (suffix_index < cumulative_sizes[i]) {
            return i;
        }
    }

    exit(-1);
}

 
node *new_node(int start, int *end) {
    int i;
    node *n = (node *) safe_malloc(sizeof(node));

    for (i = 0; i < ALPHABET_SIZE; i++)
        n->children[i] = NULL;
 
    n->suffix_link = root;
    n->start = start;
    n->end = end;
 
    n->suffix_index = -1;
    n->bit_array = (unsigned long *) safe_malloc(sizeof(unsigned long) * bit_array_size);

    for (i = 0; i < bit_array_size; i++) {
        n->bit_array[i] = 0;
    }

    return n;
}
 
int edge_length(node *n) {	
    if (n == root)	
        return 0;

    return *(n->end) - (n->start) + 1;
}
 
int walk_down(node *curr_node) {
    if (active_length >= edge_length(curr_node)) {
        active_edge += edge_length(curr_node);
        active_length -= edge_length(curr_node);
        active_node = curr_node;

        return 1;
    }
    return 0;
}
 
void extend_suffix_tree(int pos) {
    node *next, *split;

    leaf_end = pos;
 
    remaining_suffix_count++;

    last_new_node = NULL;
    
    while (remaining_suffix_count > 0) {
 
        if (active_length == 0) {
            active_edge = pos;
        }
        
        if (active_node->children[get_char_index(text[active_edge])] == NULL) {
            active_node->children[get_char_index(text[active_edge])] = new_node(pos, &leaf_end);
 
            if (last_new_node != NULL) {
                last_new_node->suffix_link = active_node;
                last_new_node = NULL;
            }
        }
        
        else {
            next = active_node->children[get_char_index(text[active_edge])];

            if (walk_down(next)) { 
                continue;
            }

            if (text[next->start + active_length] == text[pos]) {
                if (last_new_node != NULL && active_node != root) {
                    last_new_node->suffix_link = active_node;
                    last_new_node = NULL;
                }
 
                
                active_length++;
                break;
            }
 
            split_end = (int *) safe_malloc(sizeof(int));
            *split_end = next->start + active_length - 1;
            
            split = new_node(next->start, split_end);
            active_node->children[get_char_index(text[active_edge])] = split;
 
            
            split->children[get_char_index(text[pos])] = new_node(pos, &leaf_end);
            next->start += active_length;

            split->children[get_char_index(text[next->start])] = next;
 
            if (last_new_node != NULL) {
                last_new_node->suffix_link = split;
            }
 
            last_new_node = split;
        }
 
        remaining_suffix_count--;

        if (active_node == root && active_length > 0) {
            active_length--;
            active_edge = pos - remaining_suffix_count + 1;
        }
           
        
        else if (active_node != root) {
            active_node = active_node->suffix_link;
        }
    }
}

void print_path(int i, int j) {
    int k;

    for (k = i; k <= j; k++)
        printf("%c", text[k]);

    printf("\n");
}

void set_suffix_index_by_dfs(node *n, int label_height) {
    int i, leaf = 1;

    if (n == NULL) return;

    for (i = 0; i < ALPHABET_SIZE; i++) {
        if (n->children[i] != NULL) {
            leaf = 0;
            set_suffix_index_by_dfs(n->children[i], label_height + edge_length(n->children[i]));
        }
    }

    if (leaf == 1) {
        n->suffix_index = total_size - label_height;
    }
}

void helper_lcs(node *n, int *lcs, int label_height) {
    int i, count;

    for (i = 0; i < ALPHABET_SIZE; i++) {
        if (n->children[i] != NULL) {
            if (n->children[i]->suffix_index == -1) {
                helper_lcs(n->children[i], lcs, label_height + edge_length(n));
            } else {
                set_bit(n->children[i]->bit_array, get_k(n->children[i]->suffix_index));
            }

            merge_bit_arrays(n->bit_array, n->children[i]->bit_array);
        }
    }

    count = count_bits(n->bit_array);
    lcs[count] = max(lcs[count], edge_length(n) + label_height);
}

void longest_common_substrings(node *root) {
    int i, *lcs = safe_malloc(sizeof(int) * (N + 1));
    memset(lcs, 0, sizeof(int) * (N + 1));

    for (i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i] != NULL && root->children[i]->suffix_index == -1) {
            helper_lcs(root->children[i], lcs, 0);
        }
    }

    for (i = N - 1; i >= 0; i--) {
        lcs[i] = max(lcs[i], lcs[i + 1]);
    }

    for (i = 2; i <= N; i++) {
        printf("%d ", lcs[i]);
    }

    printf("\n");
    free(lcs);
}
 
void free_suffix_tree_by_post_order(node *n) {
    int i;
    
    if (n == NULL)
        return;

    for (i = 0; i < ALPHABET_SIZE; i++) {
        if (n->children[i] != NULL) {
            free_suffix_tree_by_post_order(n->children[i]);
        }
    }

    if (n->suffix_index == -1)
        free(n->end);
    
    free(n->bit_array);
    free(n);
}
 

void build_suffix_tree() {
    int i, label_height = 0;
    root_end = (int *) safe_malloc(sizeof(int));
    *root_end = - 1;
 
    root = new_node(-1, root_end);
    active_node = root; 

    for (i = 0; i < total_size; i++) {
        if (text[i] == '$') {
            text[i] = '#';
        }

        extend_suffix_tree(i);

        if (text[i] == '#') {
            text[i] = '$';
        }
    }

    set_suffix_index_by_dfs(root, label_height);
}

/* -------------- */
/* -----main----- */
/* -------------- */

int main() {
    int i;
    char **strings = NULL;

    safe_scanf("%d", &N);
    bit_array_size = ((N / 64) + ((N % 64 > 0) ? 1 : 0)); 

    strings = (char **) safe_malloc(sizeof(char *) * N);
    cumulative_sizes = (int *) safe_malloc(sizeof(int) * N);

    for (i = 0; i < N; i++) {
        strings[i] = read_line();
        total_size += strlen(strings[i]) + 1;
        cumulative_sizes[i] = total_size;
    }

    text = get_generalized_string(strings, N, total_size);
    free_strings(strings, N);

    build_suffix_tree();
    longest_common_substrings(root);

    free(text);
    free(cumulative_sizes);
    free_suffix_tree_by_post_order(root);

    return 0;
}