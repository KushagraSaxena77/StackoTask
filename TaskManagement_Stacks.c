#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_TASKS 100
#define MAX_DESCRIPTION 100
#define FILENAME "tasks.dat"

 
typedef enum {
    PENDING = 0,
    IN_PROGRESS = 1,
    COMPLETED = 2
} TaskStatus;

 
typedef struct {
    char description[MAX_DESCRIPTION];
    int year;
    int month;
    int day;
    int importance;
    TaskStatus status;
    time_t createdAt;
} Task;

 
typedef struct {
    Task tasks[MAX_TASKS];
    int top;   
} TaskStack;

 
typedef struct {
    Task tasks[MAX_TASKS];
    int top;
} UndoStack;

 
const char* monthNames[] = {
    "January", "February", "March", "April", "May", "June", "July",
    "August", "September", "October", "November", "December"
};

const char* statusNames[] = {
    "Pending", "In Progress", "Completed"
};

 
void initializeStack(TaskStack* stack);
void initializeUndoStack(UndoStack* stack);
bool pushTask(TaskStack* stack, Task task);
bool popTask(TaskStack* stack, Task* task);
bool peekTask(TaskStack* stack, Task* task);
bool isEmptyStack(TaskStack* stack);
bool isFullStack(TaskStack* stack);
bool isValidDate(int year, int month, int day);
void clearInputBuffer();
void saveStackToFile(TaskStack stack);
bool loadStackFromFile(TaskStack* stack);
void displayTask(Task task, int index);
void displayAllTasks(TaskStack stack);
void searchTasks(TaskStack stack, const char* keyword);
void drawUIHeader();
void drawUIFooter();
void drawLine(int width);
Task* getTaskAtIndex(TaskStack* stack, int index);
int getStackSize(TaskStack stack);
bool removeTaskAtIndex(TaskStack* stack, int index, UndoStack* undoStack);
bool editTaskAtIndex(TaskStack* stack, int index, Task newTask, UndoStack* undoStack);
void sortStackByDate(TaskStack* stack);
void sortStackByImportance(TaskStack* stack);
bool undoLastOperation(TaskStack* stack, UndoStack* undoStack);

 
void initializeStack(TaskStack* stack) {
    stack->top = -1;
}

 
void initializeUndoStack(UndoStack* stack) {
    stack->top = -1;
}

 
bool pushTask(TaskStack* stack, Task task) {
    if (!isFullStack(stack)) {
        stack->top++;
        stack->tasks[stack->top] = task;
        return true;
    }
    return false;
}

 
bool popTask(TaskStack* stack, Task* task) {
    if (!isEmptyStack(stack)) {
        *task = stack->tasks[stack->top];
        stack->top--;
        return true;
    }
    return false;
}

 
bool peekTask(TaskStack* stack, Task* task) {
    if (!isEmptyStack(stack)) {
        *task = stack->tasks[stack->top];
        return true;
    }
    return false;
}

 
bool isEmptyStack(TaskStack* stack) {
    return stack->top == -1;
}

 
bool isFullStack(TaskStack* stack) {
    return stack->top == MAX_TASKS - 1;
}

 
int getStackSize(TaskStack stack) {
    return stack.top + 1;
}

 
bool pushToUndoStack(UndoStack* stack, Task task) {
    if (stack->top < MAX_TASKS - 1) {
        stack->top++;
        stack->tasks[stack->top] = task;
        return true;
    }
    return false;
}

 
bool popFromUndoStack(UndoStack* stack, Task* task) {
    if (stack->top >= 0) {
        *task = stack->tasks[stack->top];
        stack->top--;
        return true;
    }
    return false;
}

 
bool undoLastOperation(TaskStack* stack, UndoStack* undoStack) {
    Task undoTask;
    if (popFromUndoStack(undoStack, &undoTask)) {
         
        pushTask(stack, undoTask);
        return true;
    }
    return false;
}

 
bool isValidDate(int year, int month, int day) {
    if (year < 1000 || year > 9999 || month < 1 || month > 12 || day < 1 || day > 31) {
        return false;
    }
    
     
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
        return false;
    }
    
     
    if (month == 2) {
        bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (isLeapYear && day > 29) {
            return false;
        } else if (!isLeapYear && day > 28) {
            return false;
        }
    }
    
    return true;
}

 
Task* getTaskAtIndex(TaskStack* stack, int index) {
    if (index >= 0 && index <= stack->top) {
        return &(stack->tasks[index]);
    }
    return NULL;
}

 
bool removeTaskAtIndex(TaskStack* stack, int index, UndoStack* undoStack) {
    if (index < 0 || index > stack->top) {
        return false;
    }
    
     
    pushToUndoStack(undoStack, stack->tasks[index]);
    
     
    TaskStack tempStack;
    initializeStack(&tempStack);
    
     
    Task task;
    for (int i = stack->top; i > index; i--) {
        popTask(stack, &task);
        pushTask(&tempStack, task);
    }
    
     
    popTask(stack, &task);
    
     
    while (!isEmptyStack(&tempStack)) {
        popTask(&tempStack, &task);
        pushTask(stack, task);
    }
    
    return true;
}

 
bool editTaskAtIndex(TaskStack* stack, int index, Task newTask, UndoStack* undoStack) {
    if (index < 0 || index > stack->top) {
        return false;
    }
    
     
    pushToUndoStack(undoStack, stack->tasks[index]);
    
     
    TaskStack tempStack;
    initializeStack(&tempStack);
    
     
    Task task;
    for (int i = stack->top; i > index; i--) {
        popTask(stack, &task);
        pushTask(&tempStack, task);
    }
    
     
    popTask(stack, &task);
    
     
    newTask.createdAt = task.createdAt;
    
     
    pushTask(stack, newTask);
    
     
    while (!isEmptyStack(&tempStack)) {
        popTask(&tempStack, &task);
        pushTask(stack, task);
    }
    
    return true;
}

 
void sortStackByDate(TaskStack* stack) {
     
    int size = getStackSize(*stack);
    Task* tasks = (Task*)malloc(size * sizeof(Task));
    
     
    TaskStack tempStack;
    initializeStack(&tempStack);
    
     
    for (int i = 0; i < size; i++) {
        popTask(stack, &tasks[i]);
    }
    
     
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (tasks[j].year > tasks[j + 1].year || 
                (tasks[j].year == tasks[j + 1].year && tasks[j].month > tasks[j + 1].month) ||
                (tasks[j].year == tasks[j + 1].year && tasks[j].month == tasks[j + 1].month && tasks[j].day > tasks[j + 1].day)) {
                Task temp = tasks[j];
                tasks[j] = tasks[j + 1];
                tasks[j + 1] = temp;
            }
        }
    }
    
     
    for (int i = size - 1; i >= 0; i--) {
        pushTask(stack, tasks[i]);
    }
    
    free(tasks);
}

 
void sortStackByImportance(TaskStack* stack) {
     
    int size = getStackSize(*stack);
    Task* tasks = (Task*)malloc(size * sizeof(Task));
    
     
    TaskStack tempStack;
    initializeStack(&tempStack);
    
     
    for (int i = 0; i < size; i++) {
        popTask(stack, &tasks[i]);
    }
    
     
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (tasks[j].importance < tasks[j + 1].importance) {
                Task temp = tasks[j];
                tasks[j] = tasks[j + 1];
                tasks[j + 1] = temp;
            }
        }
    }
    
     
    for (int i = size - 1; i >= 0; i--) {
        pushTask(stack, tasks[i]);
    }
    
    free(tasks);
}

 
void saveStackToFile(TaskStack stack) {
    FILE* file = fopen(FILENAME, "wb");
    if (file != NULL) {
        int count = getStackSize(stack);
        fwrite(&count, sizeof(int), 1, file);
        fwrite(stack.tasks, sizeof(Task), count, file);
        fclose(file);
    }
}

 
bool loadStackFromFile(TaskStack* stack) {
    FILE* file = fopen(FILENAME, "rb");
    if (file != NULL) {
        int count;
        fread(&count, sizeof(int), 1, file);
        
         
        initializeStack(stack);
        
         
        Task* tasks = (Task*)malloc(count * sizeof(Task));
        fread(tasks, sizeof(Task), count, file);
        
         
        for (int i = count - 1; i >= 0; i--) {
            pushTask(stack, tasks[i]);
        }
        
        free(tasks);
        fclose(file);
        return true;
    }
    return false;
}

 
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

 
void displayTask(Task task, int index) {
    char dateStr[20];
    sprintf(dateStr, "%s %02d, %04d", monthNames[task.month - 1], task.day, task.year);
    
    printf("| %2d | %-30s | %-17s | %10d | %-11s |\n", 
           index + 1, 
           task.description, 
           dateStr, 
           task.importance, 
           statusNames[task.status]);
}

 
void displayAllTasks(TaskStack stack) {
    int size = getStackSize(stack);
    
    if (size == 0) {
        printf("| No tasks available.                                                         |\n");
        return;
    }
    
    drawLine(80);
    printf("| %2s | %-30s | %-17s | %10s | %-11s |\n", 
           "ID", "Description", "Due Date", "Importance", "Status");
    drawLine(80);
    
    TaskStack tempStack;
    initializeStack(&tempStack);
    Task task;
    
     
    TaskStack copyStack = stack;
    
     
    Task* tasks = (Task*)malloc(size * sizeof(Task));
    
     
    for (int i = 0; i < size; i++) {
        popTask(&copyStack, &task);
        tasks[i] = task;
    }
    
     
    for (int i = size - 1; i >= 0; i--) {
        displayTask(tasks[i], size - 1 - i);
    }
    
    free(tasks);
    drawLine(80);
}

 
void searchTasks(TaskStack stack, const char* keyword) {
    bool found = false;
    drawUIHeader();
    printf("| Search Results for '%s':                                                |\n", keyword);
    drawLine(80);
    
    int size = getStackSize(stack);
    Task* tasks = (Task*)malloc(size * sizeof(Task));
    TaskStack copyStack = stack;
    Task task;
    
     
    for (int i = 0; i < size; i++) {
        popTask(&copyStack, &task);
        tasks[i] = task;
    }
    
     
    printf("| %2s | %-30s | %-17s | %10s | %-11s |\n", 
           "ID", "Description", "Due Date", "Importance", "Status");
    drawLine(80);
    
    for (int i = size - 1; i >= 0; i--) {
        if (strstr(tasks[i].description, keyword) != NULL) {
            displayTask(tasks[i], size - 1 - i);
            found = true;
        }
    }
    
    free(tasks);
    
    if (!found) {
        printf("| No tasks matching '%s' found.                                            |\n", keyword);
    }
    
    drawUIFooter();
}

 
void drawUIHeader() {
    system("clear || cls");  
    drawLine(80);
    printf("|                       TASK MANAGEMENT SYSTEM (STACK-BASED)                    |\n");
    drawLine(80);
}

void drawUIFooter() {
    drawLine(80);
    printf("| Press Enter to continue...                                                    |\n");
    drawLine(80);
    clearInputBuffer();
}

void drawLine(int width) {
    printf("+");
    for (int i = 0; i < width - 2; i++) {
        printf("-");
    }
    printf("+\n");
}

 
int main() {
    TaskStack taskStack;
    UndoStack undoStack;
    Task newTask;
    char searchKeyword[MAX_DESCRIPTION];
    int taskIndex, taskNumber;
    
    initializeStack(&taskStack);
    initializeUndoStack(&undoStack);
    
     
    if (loadStackFromFile(&taskStack)) {
        printf("Tasks loaded successfully from file.\n");
    }
    
    while (1) {
        drawUIHeader();
        printf("| Main Menu:                                                                   |\n");
        printf("| 1. Push Task (Add)                                                           |\n");
        printf("| 2. Pop Task (Remove Last)                                                    |\n");
        printf("| 3. View All Tasks                                                            |\n");
        printf("| 4. Edit Task                                                                 |\n");
        printf("| 5. Remove Specific Task                                                      |\n");
        printf("| 6. Search Tasks                                                              |\n");
        printf("| 7. Sort by Date                                                              |\n");
        printf("| 8. Sort by Importance                                                        |\n");
        printf("| 9. Undo Last Operation                                                       |\n");
        printf("| 10. Save Tasks                                                               |\n");
        printf("| 11. Exit                                                                     |\n");
        drawLine(80);
        printf("| Enter your choice: ");
        
        int choice;
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch (choice) {
            case 1:  
                drawUIHeader();
                printf("| Push New Task                                                             |\n");
                drawLine(80);
                
                printf("| Enter Task Description: ");
                scanf(" %99[^\n]", newTask.description);
                clearInputBuffer();
                
                printf("| Enter Task Due Date (YYYY MM DD): ");
                if (scanf("%d %d %d", &newTask.year, &newTask.month, &newTask.day) != 3 || 
                    !isValidDate(newTask.year, newTask.month, newTask.day)) {
                    printf("| Invalid date format. Please enter a valid date.\n");
                    drawUIFooter();
                    continue;
                }
                clearInputBuffer();
                
                printf("| Enter Task Importance (1-10): ");
                scanf("%d", &newTask.importance);
                clearInputBuffer();
                
                printf("| Task Status (0-Pending, 1-In Progress, 2-Completed): ");
                int status;
                scanf("%d", &status);
                if (status >= PENDING && status <= COMPLETED) {
                    newTask.status = (TaskStatus)status;
                } else {
                    newTask.status = PENDING;
                }
                clearInputBuffer();
                
                 
                newTask.createdAt = time(NULL);
                
                if (pushTask(&taskStack, newTask)) {
                    printf("| Task pushed successfully!\n");
                } else {
                    printf("| Task stack is full. Cannot push more tasks.\n");
                }
                
                drawUIFooter();
                break;
                
            case 2:  
                drawUIHeader();
                printf("| Pop Task                                                                  |\n");
                drawLine(80);
                
                Task poppedTask;
                if (popTask(&taskStack, &poppedTask)) {
                    printf("| Popped Task: %s\n", poppedTask.description);
                    pushToUndoStack(&undoStack, poppedTask);  
                } else {
                    printf("| Task stack is empty. Nothing to pop.\n");
                }
                
                drawUIFooter();
                break;
                
            case 3:  
                drawUIHeader();
                printf("| Current Tasks (%d/%d):                                                    |\n", 
                       getStackSize(taskStack), MAX_TASKS);
                
                displayAllTasks(taskStack);
                
                drawUIFooter();
                break;
                
            case 4:  
                drawUIHeader();
                printf("| Edit Task                                                                 |\n");
                drawLine(80);
                
                if (isEmptyStack(&taskStack)) {
                    printf("| Task stack is empty. Nothing to edit.\n");
                    drawUIFooter();
                    continue;
                }
                
                displayAllTasks(taskStack);
                
                printf("| Enter Task ID to edit: ");
                scanf("%d", &taskNumber);
                clearInputBuffer();
                
                taskIndex = taskNumber - 1;
                Task* taskToEdit = getTaskAtIndex(&taskStack, getStackSize(taskStack) - 1 - taskIndex);
                
                if (taskToEdit == NULL) {
                    printf("| Invalid Task ID. Please try again.\n");
                    drawUIFooter();
                    continue;
                }
                
                 
                Task editedTask = *taskToEdit;
                
                printf("| Current description: %s\n", editedTask.description);
                printf("| Enter new description (or press Enter to keep current): ");
                char newDesc[MAX_DESCRIPTION];
                if (fgets(newDesc, MAX_DESCRIPTION, stdin) != NULL && newDesc[0] != '\n') {
                    newDesc[strcspn(newDesc, "\n")] = 0;  
                    strcpy(editedTask.description, newDesc);
                }
                
                printf("| Current date: %s %d, %d\n", 
                       monthNames[editedTask.month - 1], editedTask.day, editedTask.year);
                printf("| Enter new date (YYYY MM DD) or 0 to keep current: ");
                int year, month, day;
                if (scanf("%d", &year) == 1 && year != 0) {
                    scanf("%d %d", &month, &day);
                    if (isValidDate(year, month, day)) {
                        editedTask.year = year;
                        editedTask.month = month;
                        editedTask.day = day;
                    } else {
                        printf("| Invalid date. Keeping current date.\n");
                    }
                }
                clearInputBuffer();
                
                printf("| Current importance: %d\n", editedTask.importance);
                printf("| Enter new importance (1-10) or 0 to keep current: ");
                int importance;
                if (scanf("%d", &importance) == 1 && importance > 0) {
                    editedTask.importance = importance;
                }
                clearInputBuffer();
                
                printf("| Current status: %s\n", statusNames[editedTask.status]);
                printf("| Enter new status (0-Pending, 1-In Progress, 2-Completed) or -1 to keep current: ");
                if (scanf("%d", &status) == 1 && status >= 0) {
                    if (status >= PENDING && status <= COMPLETED) {
                        editedTask.status = (TaskStatus)status;
                    }
                }
                clearInputBuffer();
                
                if (editTaskAtIndex(&taskStack, getStackSize(taskStack) - 1 - taskIndex, editedTask, &undoStack)) {
                    printf("| Task updated successfully!\n");
                } else {
                    printf("| Error updating task.\n");
                }
                
                drawUIFooter();
                break;
                
            case 5:  
                drawUIHeader();
                printf("| Remove Specific Task                                                      |\n");
                drawLine(80);
                
                if (isEmptyStack(&taskStack)) {
                    printf("| Task stack is empty. Nothing to remove.\n");
                    drawUIFooter();
                    continue;
                }
                
                displayAllTasks(taskStack);
                
                printf("| Enter Task ID to remove: ");
                scanf("%d", &taskNumber);
                clearInputBuffer();
                
                taskIndex = taskNumber - 1;
                int actualIndex = getStackSize(taskStack) - 1 - taskIndex;
                
                if (removeTaskAtIndex(&taskStack, actualIndex, &undoStack)) {
                    printf("| Task removed successfully!\n");
                } else {
                    printf("| Invalid Task ID or error removing task.\n");
                }
                
                drawUIFooter();
                break;
                
            case 6:  
                drawUIHeader();
                printf("| Search Tasks                                                              |\n");
                drawLine(80);
                
                printf("| Enter search keyword: ");
                scanf(" %99[^\n]", searchKeyword);
                clearInputBuffer();
                
                searchTasks(taskStack, searchKeyword);
                break;
                
            case 7:  
                sortStackByDate(&taskStack);
                drawUIHeader();
                printf("| Tasks sorted by date.                                                     |\n");
                displayAllTasks(taskStack);
                drawUIFooter();
                break;
                
            case 8:  
                sortStackByImportance(&taskStack);
                drawUIHeader();
                printf("| Tasks sorted by importance.                                               |\n");
                displayAllTasks(taskStack);
                drawUIFooter();
                break;
                
            case 9:  
                drawUIHeader();
                printf("| Undo Last Operation                                                       |\n");
                drawLine(80);
                
                if (undoLastOperation(&taskStack, &undoStack)) {
                    printf("| Last operation undone successfully!\n");
                } else {
                    printf("| Nothing to undo or error occurred.\n");
                }
                
                drawUIFooter();
                break;
                
            case 10:  
                saveStackToFile(taskStack);
                drawUIHeader();
                printf("| Tasks saved successfully!                                                 |\n");
                drawUIFooter();
                break;
                
            case 11:  
                drawUIHeader();
                printf("| Saving tasks before exit...                                               |\n");
                saveStackToFile(taskStack);
                printf("| Thank you for using the Stack-Based Task Management System!               |\n");
                drawUIFooter();
                exit(0);
                
            default:
                drawUIHeader();
                printf("| Invalid choice. Please try again.                                           |\n");
                drawUIFooter();
        }
    }
    
    return 0;
}