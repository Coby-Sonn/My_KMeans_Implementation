#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int epsilon = 0.001;

struct cord
{
    double value;
    struct cord *next;
};

struct vector
{
    struct vector *next;
    struct cord *cords;
};

/* Method that validates user provided k and returns if k is an integer by value and between (1,N) */
int validateK(char *k, int num_points)
{
    int i;
    int k_int;
    if (*k == '\0')
    {
        return 0;
    }

    for (i = 0; k[i] != '\0'; i++)
    {
        if (k[i] < '0' || k[i] > '9')
        {
            return 0;
        }
    }

    k_int = atoi(k);
    if (k_int <= 1 || k_int >= num_points)
    {
        return 0;
    }
    return k_int;
}

/*Method that validates user provided iter and returns if iter is a valid integer*/
int validateIter(char *iter)
{
    if (*iter == '-' || *iter == '+')
    {
        iter++;
    }
    if (*iter == '\0')
    {
        return 0;
    }
    while (*iter)
    {
        if (*iter < '0' || *iter > '9')
        {
            return 0;
        }
        iter++;
    }
    return 1;
}

/*cleanups*/
void freeVector(struct vector *vec)
{
    struct cord *cord, *temp_cord;
    if (!vec)
    {
        return;
    }
    cord = vec->cords;
    while (cord != NULL)
    {
        temp_cord = cord;
        cord = cord->next;
        free(temp_cord);
    }
    vec->cords = NULL;
}

/*Method that receives an array of vectors of length k and frees their memory, cords included*/
void cleanupVectors(struct vector *vectors, int k)
{
    struct cord *cord, *temp_cord;
    int i;
    for (i = 0; i < k; i++)
    {
        cord = vectors[i].cords;
        while (cord)
        {
            temp_cord = cord;
            cord = cord->next;
            free(temp_cord);
        }
        vectors[i].cords = NULL;
    }
}

/*Method that receives pointer to head of linked list of vectors and frees their memory, cords included*/
void cleanupVectorsList(struct vector *head)
{
    struct vector *temp_vec;

    while (head != NULL)
    {
        temp_vec = head;
        head = head->next;

        freeVector(temp_vec);
        free(temp_vec);
    }
}

/*Method that initiates an empty vector*/
void initialize_vector(struct vector *vec)
{
    vec->next = NULL;
    vec->cords = NULL;
}

void initialize_cord(struct cord *cord)
{
    cord->next = NULL;
}

/*Method that copies a vector*/
int copy_vector(struct vector *src, struct vector *dest)
{

    struct cord *src_cord;
    struct cord *prev;

    if (!src || !dest)
    {
        printf("An Error Has Occurred\n");
        return 0;
    }
    src_cord = src->cords;
    prev = NULL;
    dest->cords = NULL;

    while (src_cord)
    {
        struct cord *new_cord = malloc(sizeof(struct cord));
        if (!new_cord)
        {
            struct cord *temp = dest->cords;
            printf("An Error Has Occurred\n");
            while (temp)
            {
                struct cord *next = temp->next;
                free(temp);
                temp = next;
            }
            dest->cords = NULL;
            return 0;
        }
        new_cord->value = src_cord->value;
        new_cord->next = NULL;

        if (!dest->cords)
        {
            dest->cords = new_cord;
        }
        else
        {
            prev->next = new_cord;
        }

        prev = new_cord;
        src_cord = src_cord->next;
    }
    return 1;
}

/* This method receives pointers to two points of the same dimension and returns the distance between them.
The dimensions are pre-validated. */
double calculateDistance(struct vector *point1, struct vector *point2, int d)
{
    int i;
    double delta = 0.0;
    double coord_diff;
    struct cord *first_cord, *second_cord;

    if (!point1 || !point2)
    {
        return -1.0;
    }

    first_cord = point1->cords;
    second_cord = point2->cords;

    if (!first_cord || !second_cord)
    {
        return -1.0;
    }
    for (i = 0; i < d && first_cord != NULL && second_cord != NULL; i++)
    {
        coord_diff = pow(first_cord->value - second_cord->value, 2);
        first_cord = first_cord->next;
        second_cord = second_cord->next;
        delta += coord_diff;
    }

    delta = sqrt(delta);
    return delta;
}

/*Method that calculates convergence and returns 0 iff changes are under epsilon - > if 1 then we should continute iterating */
int calculateConvergence(struct vector old_centroids[], struct vector new_centroids[], int k)
{
    struct vector *old_v;
    struct vector *new_v;
    struct cord *old_c;
    struct cord *new_c;

    int i;
    for (i = 0; i < k; i++)
    {

        old_v = &old_centroids[i];
        new_v = &new_centroids[i];

        old_c = old_v->cords;
        new_c = new_v->cords;

        while (old_c != NULL && new_c != NULL)
        {
            if (fabs(old_c->value - new_c->value) >= epsilon)
            {
                return 0;
            }
            old_c = old_c->next;
            new_c = new_c->next;
        }
    }
    return 1;
}

/* Method that calculates the distance between a point to all the centroids, and returns the index of the cluster which it is closest to */
int calculateClosestCluster(struct vector centroids[], struct vector *point, int k, int d)
{
    int min_distance_idx;
    int i;
    double distance;
    double *distances = malloc(k * sizeof(double));
    if (!point || !point->cords)
    {
        return -1;
    }
    if (!distances)
    {
        return -1;
    }

    for (i = 0; i < k; i++)
    {
        if (!centroids[i].cords)
        {
            free(distances);
            return -1;
        }
        distance = calculateDistance(&centroids[i], point, d);
        if (distance < 0)
        {
            free(distances);
            return -1;
        }
        distances[i] = distance;
    }
    min_distance_idx = 0;
    for (i = 1; i < k; i++)
    {
        if (distances[i] < distances[min_distance_idx])
        {
            min_distance_idx = i;
        }
    }
    free(distances);
    return min_distance_idx;
}

/*Method that runs one iteration of the kmeans algorithm and returns a list of the updated centroids*/
struct vector *centroidIteration(int k, int d, struct vector oldcentroid[], struct vector *cur_point, int num_points)
{
    int i, p, j, l;
    int *counts;
    struct vector *new_list;
    int index_centroid;
    struct cord *new_list_cord;
    struct cord *cur_point_cord;
    struct cord *prev;
    struct vector *current_point;

    counts = calloc(k, sizeof(int));
    new_list = malloc(k * sizeof(struct vector));

    if (!counts || !new_list)
    {
        if (!counts && !new_list)
        {
            return NULL;
        }
        if (counts)
        {
            free(counts);
        }
        if (new_list)
        {
            free(new_list);
        }
        return NULL;
    }
    for (i = 0; i < k; i++)
    {
        initialize_vector(&new_list[i]);
    }
    for (i = 0; i < k; i++)
    {
        prev = NULL;
        new_list[i].next = NULL;
        new_list[i].cords = NULL;

        for (j = 0; j < d; j++)
        {
            struct cord *new_cord = malloc(sizeof(struct cord));
            if (!new_cord)
            {
                for (l = 0; l <= i; l++)
                {
                    struct cord *cord = new_list[l].cords, *temp_cord;
                    while (cord)
                    {
                        temp_cord = cord;
                        cord = cord->next;
                        free(temp_cord);
                    }
                }
                free(new_list);
                free(counts);
                return NULL;
            }
            new_cord->value = 0.0;
            new_cord->next = NULL;

            if (prev == NULL)
            {
                new_list[i].cords = new_cord;
            }
            else
            {
                prev->next = new_cord;
            }
            prev = new_cord;
        }
    }

    current_point = cur_point;
    for (p = 0; p < num_points; p++)
    {
        if (!current_point || !current_point->cords)
        {
            cleanupVectors(new_list, k);
            free(counts);
            free(new_list);
            return NULL;
        }

        index_centroid = calculateClosestCluster(oldcentroid, current_point, k, d);
        if (index_centroid < 0 || index_centroid >= k)
        {
            cleanupVectors(new_list, k);
            free(counts);
            free(new_list);
            return NULL;
        }

        new_list_cord = new_list[index_centroid].cords;
        cur_point_cord = current_point->cords;

        while (new_list_cord && cur_point_cord)
        {
            new_list_cord->value += cur_point_cord->value;
            new_list_cord = new_list_cord->next;
            cur_point_cord = cur_point_cord->next;
        }

        counts[index_centroid]++;

        current_point = current_point->next;
    }

    for (i = 0; i < k; i++)
    {
        struct cord *current_cord = new_list[i].cords;
        if (counts[i] > 0)
        {
            while (current_cord)
            {
                current_cord->value /= counts[i];
                current_cord = current_cord->next;
            }
        }
    }

    free(counts);
    return new_list;
}

/* Method that prints the points as required in the instructions */
void printRes(struct vector centroids[], int k)
{
    int i;

    for (i = 0; i < k; i++)
    {
        struct vector *curr_vec = &centroids[i];
        struct cord *curr_cord = curr_vec->cords;

        while (curr_cord != NULL)
        {
            printf("%.4f", curr_cord->value);
            if (curr_cord->next != NULL)
            {
                printf(",");
            }
            curr_cord = curr_cord->next;
        }
        printf("\n");
    }
}

/* method that receives and initial save of arguments from command line */
int main(int argc, char **argv)
{
    int k;
    int iter;
    struct vector *head_vec, *curr_vec;
    struct cord *head_cord, *curr_cord;
    double n;
    char c;
    int num_points;
    int num_coords;
    int i, iteration;
    struct vector *old_centroids;
    struct vector *new_centroids;
    int successfull_copy;
    struct cord *original_head_cord = NULL;

    /* Handle input arguments */
    if (argc == 2)
    {
        iter = 200;
    }
    else if (argc == 3)
    {
        if (validateIter(argv[2]))
        {
            iter = atoi(argv[2]);
            if (iter >= 1000 || iter <= 1)
            {
                printf("Invalid maximum iteration!\n");
                return 1;
            }
        }
        else
        {
            printf("Invalid maximum iteration!\n");
            return 1;
        }
    }
    else
    {
        printf("An Error Has Occurred\n");
        return 1;
    }

    num_points = 0;
    num_coords = 0;

    head_cord = malloc(sizeof(struct cord));
    if (!head_cord)
    {
        printf("An Error Has Occurred\n");
        return 1;
    }
    initialize_cord(head_cord);
    curr_cord = head_cord;
    original_head_cord = head_cord;

    head_vec = malloc(sizeof(struct vector));
    if (!head_vec)
    {
        free(head_cord);
        printf("An Error Has Occurred\n");
        return 1;
    }
    initialize_vector(head_vec);
    curr_vec = head_vec;
    curr_vec->next = NULL;

    while (scanf("%lf%c", &n, &c) == 2)
    {

        curr_cord->value = n;

        if (c == '\n')
        {
            curr_cord->next = NULL;
            curr_vec->cords = head_cord;
            num_points++;

            if (feof(stdin))
            {
                break;
            }

            curr_vec->next = malloc(sizeof(struct vector));
            if (!curr_vec->next)
            {
                cleanupVectorsList(head_vec);
                printf("An Error Has Occurred\n");
                return 1;
            }
            curr_vec = curr_vec->next;
            initialize_vector(curr_vec);
            head_cord = malloc(sizeof(struct cord));
            if (!head_cord)
            {
                cleanupVectorsList(head_vec);
                printf("An Error Has Occurred\n");
                return 1;
            }
            initialize_cord(head_cord);
            curr_cord = head_cord;
            continue;
        }
        curr_cord->next = malloc(sizeof(struct cord));
        if (!curr_cord->next)
        {
            cleanupVectorsList(head_vec);
            printf("An Error Has Occurred\n");
            return 1;
        }
        curr_cord = curr_cord->next;
        initialize_cord(curr_cord);
    }

    if (num_points == 0)
    {
        free(original_head_cord);
        free(head_vec);
        printf("An Error Has Occurred\n");
        return 0;
    }
    curr_cord = head_vec->cords;
    while (curr_cord)
    {
        num_coords++;
        curr_cord = curr_cord->next;
    }
    k = validateK(argv[1], num_points);

    if (k == 0)
    {
        if (head_cord)
        {
            free(head_cord);
        }
        cleanupVectorsList(head_vec);
        printf("Invalid number of clusters!\n");
        return 1;
    }
    old_centroids = malloc(k * sizeof(struct vector));
    if (!old_centroids)
    {
        if (head_cord)
        {
            free(head_cord);
        }
        cleanupVectorsList(head_vec);
        printf("An Error Has Occurred\n");
        return 1;
    }
    for (i = 0; i < k; i++)
    {
        initialize_vector(&old_centroids[i]);
    }
    curr_vec = head_vec;
    for (i = 0; i < k && curr_vec != NULL; i++)
    {
        successfull_copy = copy_vector(curr_vec, &old_centroids[i]);
        if (successfull_copy == 0)
        {
            if (head_cord)
            {
                free(head_cord);
            }

            cleanupVectors(old_centroids, k);
            cleanupVectorsList(head_vec);
            free(old_centroids);
            return 1;
        }
        curr_vec = curr_vec->next;
    }
    iteration = 0;
    while (iteration < iter)
    {
        new_centroids = centroidIteration(k, num_coords, old_centroids, head_vec, num_points);
        if (!new_centroids)
        {
            if (head_cord)
            {
                free(head_cord);
            }

            cleanupVectors(old_centroids, k);
            free(old_centroids);
            cleanupVectorsList(head_vec);
            printf("An Error Has Occurred\n");
            return 1;
        }
        if (calculateConvergence(old_centroids, new_centroids, k))
        {

            cleanupVectors(old_centroids, k);
            free(old_centroids);
            break;
        }
        cleanupVectors(old_centroids, k);
        free(old_centroids);
        old_centroids = new_centroids;

        iteration++;
    }
    printRes(new_centroids, k);
    cleanupVectors(new_centroids, k);
    free(new_centroids);
    cleanupVectorsList(head_vec);
    if (head_cord)
    {
        free(head_cord);
    }

    return 0;
}