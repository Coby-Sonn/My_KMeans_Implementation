# K-Means Clustering Implementations in Python and C

This repository contains two implementations of the K-Means clustering algorithm:
- **Python Implementation:** A simple and efficient approach to clustering using Python.
- **C Implementation:** A memory-efficient and optimized version using linked lists.

## Files
- `kmeans.py` - Python implementation of K-Means.
- `kmeans.c` - C implementation of K-Means.

## Installation
For the Python implementation, ensure you have Python installed. The C implementation requires a C compiler such as `gcc`.

## How to Run

### Running the Python Implementation

#### Prerequisites:
- Ensure Python is installed (recommended version: 3.x).
- Prepare a dataset file containing points, where each line represents a data point with comma-separated coordinates.

#### Steps:
1. Open a terminal and navigate to the repository folder.
2. Run the Python script using:
   ```sh
   python kmeans.py <k> [<max_iter>] <input_file>
   ```
   - `<k>`: Number of clusters (integer > 1 and < N, where N is the number of points).
   - `<max_iter>`: (Optional) Maximum number of iterations. Default is 200, system's max is 1000.
   - `<input_file>`: Path to the dataset file.

#### Example:
```sh
python3 kmeans.py 3 100 data.txt
```

### Running the C Implementation

#### Prerequisites:
- Ensure `gcc` is installed.
- Prepare a dataset file containing points, where each line represents a data point with comma-separated coordinates.

#### Steps:
1. Open a terminal and navigate to the repository folder.
2. Compile the C file using:
   ```sh
   $gcc -ansi -Wall -Wextra -Werror -pedantic-errors kmeans.c -o kmeans -lm
   ```
3. Run the compiled program using:
   ```sh
   ./kmeans <k> [<max_iter>] < input_file
   ```
   - `<k>`: Number of clusters (integer > 1 and < N, where N is the number of points).
   - `<max_iter>`: (Optional) Maximum number of iterations. Default is 200, system's max is 1000.
   - `<input_file>`: Provide input data via standard input (use `<` operator).

#### Example:
```sh
./kmeans 3 300 < data.txt
```

## Output Format
Both implementations print the final cluster centroids, with each centroid on a new line, formatted to 4 decimal places.

## Notes
- Ensure the dataset file follows the correct format (each line contains comma-separated numerical values representing coordinates).
- If invalid parameters are provided, an error message will be displayed.

## License
This project is released under the MIT License.

