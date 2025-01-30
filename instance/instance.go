package instance

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"strconv"
	"strings"
)

type Instance struct {
	J  int
	M  int
	P  []int
	Δ  [][]int
	UB int
}

func NewInstanceFromFile(filename string) *Instance {

	f, err := os.Open(filename)
	if err != nil {
		log.Fatal(err)
	}
	defer f.Close()

	sc := bufio.NewScanner(f)

	sc.Scan()
	M, err := strconv.Atoi(sc.Text())
	if err != nil {
		log.Fatal(err)
	}

	sc.Scan()
	J, err := strconv.Atoi(sc.Text())
	if err != nil {
		log.Fatal(err)
	}

	P := make([]int, M+1)
	sc.Scan()
	i := 1
	for _, v := range strings.Split(sc.Text(), " ") {
		if P[i], err = strconv.Atoi(v); err != nil {
			log.Fatal(err)
		}
		i++
	}

	Δ := make([][]int, M+2)
	for i := 0; i < M+2; i++ {
		Δ[i] = make([]int, M+2)
		sc.Scan()
		j := 0
		for _, v := range strings.Split(sc.Text(), " ") {
			if Δ[i][j], err = strconv.Atoi(v); err != nil {
				log.Fatal(err)
			}
			j++
		}
	}

	if err := sc.Err(); err != nil {
		log.Fatal(err)
	}

	ins := &Instance{J, M, P, Δ, -1}
	ins.UB = ins.ComputeUB()
	return ins
}

func (ins *Instance) String() string {
	ΔAsString := "[\n"
	for _, v := range ins.Δ {
		ΔAsString += fmt.Sprintf("\t%v\n", v)
	}
	ΔAsString += "  ]"
	return fmt.Sprintf("Instance {\n  J = %d\n  M = %d\n  P = %v\n  Δ = %s\n  UB = %d\n}",
		ins.J, ins.M, ins.P, ΔAsString, ins.UB)
}

func (ins *Instance) ComputeUB() int {

	totalProcessingTime := 0
	for i := 1; i < len(ins.P); i++ {
		totalProcessingTime += ins.P[i]
	}
	totalProcessingTime *= ins.J

	totalForwardTravelTime := 0
	for i := 0; i < ins.M+1; i++ {
		totalForwardTravelTime += ins.Δ[i][i+1]
	}
	totalForwardTravelTime *= ins.J

	totalBackwardTravelTime := ins.Δ[ins.M+1][0] * (ins.J - 1)

	return totalProcessingTime + totalForwardTravelTime + totalBackwardTravelTime
}
