import sys
import math
import os

# Global epsilon as defined in HW1 instructions. 
EPSILON = 0.001

""" Method that validates user provided k and returns if k is an integer by value and between (1,N) """
def validateK(k, N):
    try:
        k = float(k)
        return 1 < k < N and k.is_integer()
    except:
        return False

""" Method that validates user provided iter and returns if iter is an integer by value and between (1,1000) """    
def validateIter(iter):
    try:
        iter = float(iter)
        return iter.is_integer() and 1 < iter < 1000
    except:
        return False

""" Method that reads the data from the file in path file_path. The method receives a validated legal path to an existing file.
    @return: array of points = [point1, point2, ... pointN] s.t: point = [c1, c2, c3, ..., cd] 
"""
def readData(file_path):
    points = []
    with open(file_path, 'r') as file:
        for line in file:
            point = [float(coordinate) for coordinate in line.split(',')]
            points.append(point)
            
    return points

""" Method that checks if the file with file_name provided by user is an existing file. returns file path if True, None otherwise
@return: string file_path that has the data """
def isLegalFile(file_name):
    current_directory = os.getcwd()
    path = os.path.join(current_directory, file_name)
    # if os.path.isfile(path) and path.endswith(".txt"):
    #     return path
    if os.path.isfile(path):
        return path
    else:
        # path = path + ".txt"
        # print (path)
        if os.path.isfile(path+".txt"):
            return path+".txt"
        return None
    
""" Method that gets two vectors and calculates the Euclidean Distance between them.
@return float of the distance"""
def calculateDistance(point1, point2):
    delta = 0.0
    delta_point1_point2_sqaure=[]
    for i in range(len(point1)):
        delta+=float(pow(point1[i]-point2[i],2)) #pow return double
    delta = math.sqrt(delta)
    # delta = "%.4f" % delta
    return float(delta)

""" Method that calculates convergence and returns False iff changes are under Epsilon 
-> if True then continue looping """
def calculateConvergence(old_centroids, new_centroids, d):
    for i in range(len(old_centroids)):
        for coordinate in range(d):
            if abs(new_centroids[i][coordinate] - old_centroids[i][coordinate]) >= EPSILON:
                return True
    return False

"""Method that calculates the distance between a point and the existing clusters, and returns the index of the cluster which it is closest to."""
def calculateClosestCluster(centroids, point):
    distances = [(calculateDistance(centroids[i], point), i) for i in range(len(centroids))]
    min_distance_idx = min(distances, key=lambda x: x[0])[1]
    return min_distance_idx

"""Method that runs one iteration of the kmeans iteration and returns a list of the updated centroids"""
def centroidIteration(k, d, old_centroids, points):
    new_list = [[0.0 for _ in range(d)] for _ in range(k)]
    counts = [0] * k
    for point in points:
        centroid_to_add_to = calculateClosestCluster(old_centroids, point)
        for i in range(d):
            new_list[centroid_to_add_to][i] += point[i]
        counts[centroid_to_add_to] += 1
    new_centroids = [
        [coordinate_sum / counts[i] if counts[i] > 0 else 0.0 
        for coordinate_sum in new_list[i]]
        for i in range(len(new_list))
        ]

    return new_centroids

"""Method that prints the final centroids as required in instructions"""
def printRes(centroids):
    for point in centroids:
        line = ""
        for coordinate in point:
            line += "{:.4f},".format(coordinate) 
        print (line[:-1])

"""Method that runs the kmeans algorithm"""
def kmeans(k, iter, file_name):

    if not validateIter(iter): 
        print ("Invalid maximum iteration!")
        sys.exit(1)

    iter = int(iter)

    data_path = isLegalFile(file_name)
    if data_path is None:
        print("An Error Has Occurred")
        sys.exit(1)

    points = readData(data_path) 
    N = len(points)  
    d = len(points[-1])

    if not validateK(k, N):
        print ("Invalid number of clusters!")
        sys.exit(1)

    k = int(k)
    old_centroids = [points[i] for i in range(k)]
    new_centroids = centroidIteration(k, d, old_centroids, points)
    iteration = 1
    while calculateConvergence(old_centroids, new_centroids, d) and iteration < iter :
        old_centroids = new_centroids
        new_centroids = centroidIteration(k, d, old_centroids, points)
        iteration += 1

    return new_centroids
        
if __name__ == "__main__":
    try:
        if len(sys.argv) < 3:  # check if there are at least 2 args (clusters + file)
            print("An Error Has Occurred")
            sys.exit(1)  

        try:
            k = sys.argv[1]  # number of clusters
            # check if iterations is provided
            if len(sys.argv) == 4:
                iter = sys.argv[2]  # read iterations if provided 
                file_name = sys.argv[3]   
            elif len(sys.argv) == 3:  
                iter = 200  # default value for iterations
                file_name = sys.argv[2] 
            else:
                print("An Error Has Occurred")
                sys.exit(1)
        except ValueError:
            # print("Error: <clusters> and <iterations> must be integers.")
            print("An Error Has Occurred")
            sys.exit(1)
        
        # Call kmeans method and print result
        printRes(kmeans(k, iter, file_name))

    except Exception as e:
        print("An Error Has Occurred")
        sys.exit(1)
