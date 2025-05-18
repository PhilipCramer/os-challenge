# os-challenge-group-08
Each Branch contains a different optimization of the assingment in 02335 Operating Systems at DTU

This branch is is based around the `branch_prediction.c` which is a result of reverseengineering the decompiled test binary provided in the course.
In order to accurately predict the result the program does a 2 stage bruteforce to acquire the seed used for generating requests. The test binary will eithe use a seed provided by the user or if none is provided a time seed, therfor the program starts by assuming a time seed as this has the lowest search space assuming the test binary an the program have the me local time. If it fails to aquire the seed through that it proceeds to trying to bruteforce the seed starting from 0.
In order to avoid spending too much time guessing the seed, the program only tries up to a seed value of 100.000.000 as this was found to be an acceptable loss in case no seed was found.
