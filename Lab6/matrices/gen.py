from scipy import random, linalg
import numpy
matrixSize = 500 
A = random.rand(matrixSize,matrixSize)
B = numpy.dot(A,A.transpose())
for row in B:
	rowT = []
	for cell in row:
		rowT.append(str(cell))
	print ",".join(rowT)
