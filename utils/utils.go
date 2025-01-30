package utils

import (
	"fmt"
	"slices"
	"time"
)

type StateI interface {
	IsFinal() bool
	String() string
}

type SolveResult struct {
	BestObjVal    int
	ExecutionTime time.Duration
	OptimalSol    []StateI
}

func (r SolveResult) Print(solverName string, printOptimalSol bool) {

	fmt.Println(solverName, r.BestObjVal, r.ExecutionTime)

	if printOptimalSol {
		fmt.Println("optimal solution =")
		fmt.Println()
		for _, s := range r.OptimalSol {
			fmt.Println("\t", s)
		}
	}
	fmt.Println()
}

func BuildOptimalSol(bestState StateI, predecessor map[StateI]StateI) []StateI {
	optimalSol := make([]StateI, 0)
	curState := bestState
	for curState != nil {
		optimalSol = append(optimalSol, curState)
		curState = predecessor[curState]
	}
	slices.Reverse(optimalSol)
	return optimalSol
}

type InstanceType struct {
	J int
	M int
	R float64
}

var InsToPoolSize map[InstanceType]uint

func InsToPoolSizeInit() {
	InsToPoolSize = make(map[InstanceType]uint)
	// tunato con seed 0...4
	InsToPoolSize[InstanceType{4, 4, 0.1}] = 6
	InsToPoolSize[InstanceType{4, 4, 0.4}] = 7
	InsToPoolSize[InstanceType{4, 4, 0.8}] = 7
	InsToPoolSize[InstanceType{4, 4, 1.1}] = 8
	InsToPoolSize[InstanceType{4, 4, 1.4}] = 8
	InsToPoolSize[InstanceType{4, 4, 1.8}] = 8
	InsToPoolSize[InstanceType{4, 4, 2.1}] = 8
	InsToPoolSize[InstanceType{4, 4, 2.5}] = 9
	InsToPoolSize[InstanceType{4, 4, 3.0}] = 9
	InsToPoolSize[InstanceType{4, 4, 3.5}] = 7
	InsToPoolSize[InstanceType{4, 4, 4.0}] = 8
	InsToPoolSize[InstanceType{5, 5, 0.1}] = 12
	InsToPoolSize[InstanceType{5, 5, 0.4}] = 13
	InsToPoolSize[InstanceType{5, 5, 0.8}] = 12
	InsToPoolSize[InstanceType{5, 5, 1.1}] = 12
	InsToPoolSize[InstanceType{5, 5, 1.4}] = 13
	InsToPoolSize[InstanceType{5, 5, 1.8}] = 13
	InsToPoolSize[InstanceType{5, 5, 2.1}] = 18
	InsToPoolSize[InstanceType{5, 5, 2.5}] = 18
	InsToPoolSize[InstanceType{5, 5, 3.0}] = 20
	InsToPoolSize[InstanceType{5, 5, 3.5}] = 20
	InsToPoolSize[InstanceType{5, 5, 4.0}] = 25
	InsToPoolSize[InstanceType{6, 6, 0.1}] = 21
	InsToPoolSize[InstanceType{6, 6, 0.4}] = 22
	InsToPoolSize[InstanceType{6, 6, 0.8}] = 22
	InsToPoolSize[InstanceType{6, 6, 1.1}] = 25
	InsToPoolSize[InstanceType{6, 6, 1.4}] = 24
	InsToPoolSize[InstanceType{6, 6, 1.8}] = 28
	InsToPoolSize[InstanceType{6, 6, 2.1}] = 32
	InsToPoolSize[InstanceType{6, 6, 2.5}] = 33
	InsToPoolSize[InstanceType{6, 6, 3.0}] = 46
	InsToPoolSize[InstanceType{6, 6, 3.5}] = 57
	InsToPoolSize[InstanceType{6, 6, 4.0}] = 57
	InsToPoolSize[InstanceType{7, 7, 0.1}] = 38
	InsToPoolSize[InstanceType{7, 7, 0.4}] = 42
	InsToPoolSize[InstanceType{7, 7, 0.8}] = 44
	InsToPoolSize[InstanceType{7, 7, 1.1}] = 43
	InsToPoolSize[InstanceType{7, 7, 1.4}] = 44
	InsToPoolSize[InstanceType{7, 7, 1.8}] = 59
	InsToPoolSize[InstanceType{7, 7, 2.1}] = 59
	InsToPoolSize[InstanceType{7, 7, 2.5}] = 67
	InsToPoolSize[InstanceType{7, 7, 3.0}] = 78
	InsToPoolSize[InstanceType{7, 7, 3.5}] = 95
	InsToPoolSize[InstanceType{7, 7, 4.0}] = 124
	InsToPoolSize[InstanceType{8, 8, 0.1}] = 78
	InsToPoolSize[InstanceType{8, 8, 0.4}] = 77
	InsToPoolSize[InstanceType{8, 8, 0.8}] = 80
	InsToPoolSize[InstanceType{8, 8, 1.1}] = 84
	InsToPoolSize[InstanceType{8, 8, 1.4}] = 97
	InsToPoolSize[InstanceType{8, 8, 1.8}] = 109
	InsToPoolSize[InstanceType{8, 8, 2.1}] = 119
	InsToPoolSize[InstanceType{8, 8, 2.5}] = 145
	InsToPoolSize[InstanceType{8, 8, 3.0}] = 227
	InsToPoolSize[InstanceType{8, 8, 3.5}] = 228
	InsToPoolSize[InstanceType{8, 8, 4.0}] = 335
	InsToPoolSize[InstanceType{9, 9, 0.1}] = 146
	InsToPoolSize[InstanceType{9, 9, 0.4}] = 134
	InsToPoolSize[InstanceType{9, 9, 0.8}] = 151
	InsToPoolSize[InstanceType{9, 9, 1.1}] = 161
	InsToPoolSize[InstanceType{9, 9, 1.4}] = 165
	InsToPoolSize[InstanceType{9, 9, 1.8}] = 203
	InsToPoolSize[InstanceType{9, 9, 2.1}] = 277
	InsToPoolSize[InstanceType{9, 9, 2.5}] = 261
	InsToPoolSize[InstanceType{9, 9, 3.0}] = 370
	InsToPoolSize[InstanceType{9, 9, 3.5}] = 570
	InsToPoolSize[InstanceType{9, 9, 4.0}] = 777
	InsToPoolSize[InstanceType{10, 10, 0.1}] = 279
	InsToPoolSize[InstanceType{10, 10, 0.4}] = 264
	InsToPoolSize[InstanceType{10, 10, 0.8}] = 284
	InsToPoolSize[InstanceType{10, 10, 1.1}] = 314
	InsToPoolSize[InstanceType{10, 10, 1.4}] = 308
	InsToPoolSize[InstanceType{10, 10, 1.8}] = 415
	InsToPoolSize[InstanceType{10, 10, 2.1}] = 442
	InsToPoolSize[InstanceType{10, 10, 2.5}] = 524
	InsToPoolSize[InstanceType{10, 10, 3.0}] = 841
	InsToPoolSize[InstanceType{10, 10, 3.5}] = 1123
	InsToPoolSize[InstanceType{10, 10, 4.0}] = 1642
	InsToPoolSize[InstanceType{11, 11, 0.1}] = 553
	InsToPoolSize[InstanceType{11, 11, 0.4}] = 507
	InsToPoolSize[InstanceType{11, 11, 0.8}] = 528
	InsToPoolSize[InstanceType{11, 11, 1.1}] = 598
	InsToPoolSize[InstanceType{11, 11, 1.4}] = 628
	InsToPoolSize[InstanceType{11, 11, 1.8}] = 678
	InsToPoolSize[InstanceType{11, 11, 2.1}] = 893
	InsToPoolSize[InstanceType{11, 11, 2.5}] = 1068
	InsToPoolSize[InstanceType{11, 11, 3.0}] = 1325
	InsToPoolSize[InstanceType{11, 11, 3.5}] = 2030
	InsToPoolSize[InstanceType{11, 11, 4.0}] = 2756
	InsToPoolSize[InstanceType{12, 12, 0.1}] = 1249
	InsToPoolSize[InstanceType{12, 12, 0.4}] = 1087
	InsToPoolSize[InstanceType{12, 12, 0.8}] = 1151
	InsToPoolSize[InstanceType{12, 12, 1.1}] = 1102
	InsToPoolSize[InstanceType{12, 12, 1.4}] = 1151
	InsToPoolSize[InstanceType{12, 12, 1.8}] = 1465
	InsToPoolSize[InstanceType{12, 12, 2.1}] = 1737
	InsToPoolSize[InstanceType{12, 12, 2.5}] = 2037
	InsToPoolSize[InstanceType{12, 12, 3.0}] = 2861
	InsToPoolSize[InstanceType{12, 12, 3.5}] = 3225
	InsToPoolSize[InstanceType{12, 12, 4.0}] = 5439
	InsToPoolSize[InstanceType{13, 13, 0.1}] = 2228
	InsToPoolSize[InstanceType{13, 13, 0.4}] = 1996
	InsToPoolSize[InstanceType{13, 13, 0.8}] = 2168
	InsToPoolSize[InstanceType{13, 13, 1.1}] = 2290
	InsToPoolSize[InstanceType{13, 13, 1.4}] = 2316
	InsToPoolSize[InstanceType{13, 13, 1.8}] = 3230
	InsToPoolSize[InstanceType{13, 13, 2.1}] = 3695
	InsToPoolSize[InstanceType{13, 13, 2.5}] = 3947
	InsToPoolSize[InstanceType{13, 13, 3.0}] = 6250
	InsToPoolSize[InstanceType{13, 13, 3.5}] = 10140
	InsToPoolSize[InstanceType{13, 13, 4.0}] = 18024
	InsToPoolSize[InstanceType{14, 14, 0.1}] = 4427
	InsToPoolSize[InstanceType{14, 14, 0.4}] = 4121
	InsToPoolSize[InstanceType{14, 14, 0.8}] = 4197
	InsToPoolSize[InstanceType{14, 14, 1.1}] = 4227
	InsToPoolSize[InstanceType{14, 14, 1.4}] = 4594
	InsToPoolSize[InstanceType{14, 14, 1.8}] = 6126
	InsToPoolSize[InstanceType{14, 14, 2.1}] = 6289
	InsToPoolSize[InstanceType{14, 14, 2.5}] = 7449
	InsToPoolSize[InstanceType{14, 14, 3.0}] = 11859
	InsToPoolSize[InstanceType{14, 14, 3.5}] = 21756
	InsToPoolSize[InstanceType{14, 14, 4.0}] = 28425
	InsToPoolSize[InstanceType{15, 15, 0.1}] = 8829
	InsToPoolSize[InstanceType{15, 15, 0.4}] = 7996
	InsToPoolSize[InstanceType{15, 15, 0.8}] = 7755
	InsToPoolSize[InstanceType{15, 15, 1.1}] = 8545
	InsToPoolSize[InstanceType{15, 15, 1.4}] = 9116
	InsToPoolSize[InstanceType{15, 15, 1.8}] = 12455
	InsToPoolSize[InstanceType{15, 15, 2.1}] = 14974
	InsToPoolSize[InstanceType{15, 15, 2.5}] = 18223
	InsToPoolSize[InstanceType{15, 15, 3.0}] = 25580
	InsToPoolSize[InstanceType{15, 15, 3.5}] = 61341
	InsToPoolSize[InstanceType{15, 15, 4.0}] = 67892
	InsToPoolSize[InstanceType{16, 16, 0.1}] = 17397
	InsToPoolSize[InstanceType{16, 16, 0.4}] = 15743
	InsToPoolSize[InstanceType{16, 16, 0.8}] = 15890
	InsToPoolSize[InstanceType{16, 16, 1.1}] = 17186
	InsToPoolSize[InstanceType{16, 16, 1.4}] = 17911
	InsToPoolSize[InstanceType{16, 16, 1.8}] = 21202
	InsToPoolSize[InstanceType{16, 16, 2.1}] = 24215
	InsToPoolSize[InstanceType{16, 16, 2.5}] = 34396
	InsToPoolSize[InstanceType{16, 16, 3.0}] = 57704
	InsToPoolSize[InstanceType{16, 16, 3.5}] = 52958
	InsToPoolSize[InstanceType{16, 16, 4.0}] = 76244

	// fisso J a 10 e r = 1.8

	InsToPoolSize[InstanceType{10, 4, 1.8}] = 7
	InsToPoolSize[InstanceType{10, 5, 1.8}] = 13
	InsToPoolSize[InstanceType{10, 6, 1.8}] = 29
	InsToPoolSize[InstanceType{10, 7, 1.8}] = 55
	InsToPoolSize[InstanceType{10, 8, 1.8}] = 97
	InsToPoolSize[InstanceType{10, 9, 1.8}] = 190
	// InsToPoolSize[InstanceType{10, 10, 1.8}] = 415
	InsToPoolSize[InstanceType{10, 11, 1.8}] = 665
	InsToPoolSize[InstanceType{10, 12, 1.8}] = 1669
	InsToPoolSize[InstanceType{10, 13, 1.8}] = 2618
	InsToPoolSize[InstanceType{10, 14, 1.8}] = 5211
	InsToPoolSize[InstanceType{10, 15, 1.8}] = 10971
	InsToPoolSize[InstanceType{10, 16, 1.8}] = 18969
	InsToPoolSize[InstanceType{10, 17, 1.8}] = 33975

	// fisso M a 10 e r = 1.8

	InsToPoolSize[InstanceType{4, 10, 1.8}] = 116
	InsToPoolSize[InstanceType{5, 10, 1.8}] = 199
	InsToPoolSize[InstanceType{6, 10, 1.8}] = 252
	InsToPoolSize[InstanceType{7, 10, 1.8}] = 372
	InsToPoolSize[InstanceType{8, 10, 1.8}] = 367
	InsToPoolSize[InstanceType{9, 10, 1.8}] = 356
	// InsToPoolSize[InstanceType{10, 10, 1.8}] = 415
	InsToPoolSize[InstanceType{11, 10, 1.8}] = 440
	InsToPoolSize[InstanceType{12, 10, 1.8}] = 390
	InsToPoolSize[InstanceType{13, 10, 1.8}] = 379
	InsToPoolSize[InstanceType{14, 10, 1.8}] = 357
	InsToPoolSize[InstanceType{15, 10, 1.8}] = 368
	InsToPoolSize[InstanceType{16, 10, 1.8}] = 440
	InsToPoolSize[InstanceType{17, 10, 1.8}] = 369
}
