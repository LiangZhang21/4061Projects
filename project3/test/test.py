import multiprocessing as mp
import os
import sys

def validate(testcase):
	expectedPath = "test/expected/" + testcase + ".txt"
	expectedResult = {}
	outputPath = "output/result.txt"
	outputResult = {}

	with open(expectedPath, 'r') as f:
		for line in f:
			key, value = line.strip().split(' ')
			expectedResult[key] = value

	with open(outputPath) as f:
		for line in f:
			key, value = line.strip().split(' ')
			outputResult[key] = value

	if expectedResult == outputResult:
		print("Pass test " + testcase)
	else:
		s = set(expectedResult) - set(outputResult)
		if len(s) != 0:
			print("Key difference : expected - observed")
			print(s)
		
		s = set(outputResult) - set(expectedResult)
		if len(s) != 0:
			print("Key difference : observed - expected")
			print(s)

		valueDiff = False
		for key in expectedResult.keys():
			if key in outputResult.keys() and expectedResult[key] != outputResult[key]:
				if valueDiff == False:
					valueDiff = True
					print("Value difference: expected val - observed")
				print(key, " expected: ", expectedResult[key], " but observed: ", outputResult[key])

def runCode(cmds):
	os.system(cmds)

def procTerm(cmds, testcase):
	p = mp.Process(target=runCode, args=(cmds,))
	p.start()
	p.join(20)
	if p.is_alive():
		print("Terminating")
		p.terminate()
		p.join()
	validate(testcase)

if __name__ == '__main__':
	basicCMD = "./mapreduce 10 test/"
	for arg in sys.argv[1:]:
		print("Testcase " + arg + " starts")
		cmd = basicCMD + arg + "/F" + arg[1:] + ".txt -p"
		print(cmd)
		try:
			procTerm(cmd, arg)
		except Exception as e:
			print(e)
			print("Program terminated due to error or timeout!!!")
		print("================================================")
	print("Testing ended....")