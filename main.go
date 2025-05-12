package main

import (
	"fmt"
	"log"
	"strings"

	"github.com/nemolino/AGV-Scheduling/instance"
	"github.com/nemolino/AGV-Scheduling/solverDP1"
	"github.com/nemolino/AGV-Scheduling/solverDP2"
	"github.com/nemolino/AGV-Scheduling/utils"
)

func main() {

	utils.InsToPoolSizeInit()

	// for JM := 4; JM <= 10; JM++ {
	// 	for _, r := range []float64{0.1, 0.4, 0.8, 1.1, 1.4, 1.8, 2.1, 2.5, 3.0, 3.5, 4.0} {
	// 		for seed := 0; seed < 5; seed++ {

	// 			filename := fmt.Sprintf("./dataset_J_equals_M/J_%02d_M_%02d_r_%.1f_seed_%02d.dat", JM, JM, r, seed)
	// 			ins := instance.NewInstanceFromFile(filename)

	// 			fmt.Println(filename)
	// 			fmt.Println()

	// 			fmt.Println(ins.UB, "UB")
	// 			fmt.Println()

	// 			r1 := solverDP1.Solve(ins)
	// 			r1.Print("solverDP1", false)

	// 			r2 := solverDP2.Solve(ins, utils.InstanceType{J: JM, M: JM, R: r})
	// 			r2.Print("solverDP2", false)

	// 			if r1.BestObjVal != r2.BestObjVal {
	// 				log.Fatal("ERROR : DP1 and DP2 found a different z*")
	// 			}

	// 			fmt.Println(strings.Repeat("-", 80))
	// 			fmt.Println()
	// 		}
	// 	}
	// }

	ins := instance.NewInstanceFromFile(
		"./dataset_J_equals_M/J_14_M_14_r_2.1_seed_00.dat",
	)

	fmt.Println(ins.UB, "UB")
	fmt.Println()

	r2 := solverDP2.Solve(ins, utils.InstanceType{J: 14, M: 14, R: 2.1})
	r2.Print("solverDP2", false)

	// fixedMExperiment()
	// fixedJExperiment()

}

func fixedMExperiment() {

	for J := 4; J <= 17; J++ {
		for seed := 0; seed < 5; seed++ {

			filename := fmt.Sprintf("./dataset_fixed_M/J_%02d_M_10_r_1.8_seed_%02d.dat", J, seed)
			ins := instance.NewInstanceFromFile(filename)

			fmt.Println(filename)
			fmt.Println()

			r1 := solverDP1.Solve(ins)
			r1.Print("solverDP1", false)

			r2 := solverDP2.Solve(ins, utils.InstanceType{J: J, M: 10, R: 1.8})
			r2.Print("solverDP2", false)

			if r1.BestObjVal != r2.BestObjVal {
				log.Fatal("ERROR : DP1 and DP2 found a different z*")
			}

			fmt.Println(strings.Repeat("-", 80))
			fmt.Println()
		}
	}
}

func fixedJExperiment() {

	for M := 4; M <= 17; M++ {
		for seed := 0; seed < 5; seed++ {

			filename := fmt.Sprintf("./dataset_fixed_J/J_10_M_%02d_r_1.8_seed_%02d.dat", M, seed)
			ins := instance.NewInstanceFromFile(filename)

			fmt.Println(filename)
			fmt.Println()

			r1 := solverDP1.Solve(ins)
			r1.Print("solverDP1", false)

			r2 := solverDP2.Solve(ins, utils.InstanceType{J: 10, M: M, R: 1.8})
			r2.Print("solverDP2", false)

			if r1.BestObjVal != r2.BestObjVal {
				log.Fatal("ERROR : DP1 and DP2 found a different z*")
			}

			fmt.Println(strings.Repeat("-", 80))
			fmt.Println()
		}
	}
}
