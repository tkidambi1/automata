#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 256
#define MAX_STATES 100
#define MAX_ALPHABET_SIZE 26

// Function to ignore comments
char* ignore_comments(char* text) {
    char* comment_start = strchr(text, ';');
    if (comment_start != NULL) {
        *comment_start = '\0';
    }
    return text;
}

// Function to read the NFA from a file
void read_nfa(const char* file_path, char* alphabet, int** transition_table, int* num_states, int** accept_states, int* num_accept_states) {
    FILE* file = fopen(file_path, "r");
    char line[MAX_LINE_LENGTH];

    fgets(line, MAX_LINE_LENGTH, file);
    strcpy(alphabet, ignore_comments(line));

    fgets(line, MAX_LINE_LENGTH, file);
    *num_states = atoi(ignore_comments(line));

    // Allocate transition table
    *transition_table = malloc((*num_states) * 2 * sizeof(int));
    for (int i = 0; i < *num_states; i++) {
        fgets(line, MAX_LINE_LENGTH, file);
        ignore_comments(line);

        // Parse transitions
        char* token = strtok(line, " ");
        for (int j = 0; j < 2; j++) {
            if (token != NULL && strcmp(token, "-") != 0) {
                (*transition_table)[i * 2 + j] = atoi(token);
            } else {
                (*transition_table)[i * 2 + j] = -1; // No transition
            }
            token = strtok(NULL, " ");
        }
    }

    // Read accept states
    fgets(line, MAX_LINE_LENGTH, file);
    ignore_comments(line);
    char* token = strtok(line, " ");
    while (token != NULL) {
        (*accept_states)[*num_accept_states] = atoi(token);
        (*num_accept_states)++;
        token = strtok(NULL, " ");
    }

    fclose(file);
}

bool is_accepted(const char* s, char* alphabet, int* transition_table, int num_states, int* accept_states, int num_accept_states) {
    bool current_states[MAX_STATES] = {false};
    current_states[0] = true; // Start at state 0

    for (int i = 0; s[i] != '\0'; i++) {
        char c = s[i];
        bool next_states[MAX_STATES] = {false};

        // Find the index of the character in the alphabet
        char* char_index = strchr(alphabet, c);
        if (char_index == NULL) {
            return false; // Character not in alphabet
        }
        int index = char_index - alphabet;

        // Update states
        for (int state = 0; state < num_states; state++) {
            if (current_states[state]) {
                int next_state = transition_table[state * 2 + index];
                if (next_state != -1) {
                    next_states[next_state] = true;
                }
            }
        }

        memcpy(current_states, next_states, sizeof(current_states));
    }

    // Check if any current state is an accept state
    for (int i = 0; i < num_accept_states; i++) {
        if (current_states[accept_states[i]]) {
            return true;
        }
    }

    return false;
}

int main() {
    char alphabet[MAX_ALPHABET_SIZE];
    int* transition_table = NULL;
    int num_states = 0;
    int* accept_states = malloc(MAX_LINE_LENGTH * sizeof(int));
    int num_accept_states = 0;

    read_nfa("nfa.txt", alphabet, &transition_table, &num_states, &accept_states, &num_accept_states);

    char input[MAX_LINE_LENGTH];
    printf("Enter a string (or 'Done' to quit): \n");
    while (fgets(input, MAX_LINE_LENGTH, stdin)) {
        input[strcspn(input, "\n")] = 0; // Remove newline character

        if (strcmp(input, "Done") == 0) {
            break;
        }

        if (is_accepted(input, alphabet, transition_table, num_states, accept_states, num_accept_states)) {
            printf("String '%s' is accepted.\n", input);
        } else {
            printf("String '%s' is rejected.\n", input);
        }

        printf("Enter a string (or 'Done' to quit): \n");
    }

    free(transition_table);
    free(accept_states);
    return 0;
}