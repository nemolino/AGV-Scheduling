package solverDP1

import (
	"time"

	"github.com/nemolino/AGV-Scheduling/instance"
	"github.com/nemolino/AGV-Scheduling/utils"
)

var J int // package global
var M int // package global

func Solve(ins *instance.Instance) utils.SolveResult {

	J, M = ins.J, ins.M
	P, Δ := ins.P, ins.Δ

	// EXTENDED_STATES_COUNT := 0

	start := time.Now()

	var bestObjVal int = ins.UB
	var bestState *state = nil

	wInitialState := make([]int, M+2)
	wInitialState[0] = J
	for j := 1; j <= M; j++ {
		wInitialState[j] = -1
	}
	wInitialState[M+1] = 0
	initialState := newState(0, 0, wInitialState)

	predecessor := make(map[utils.StateI]utils.StateI)
	predecessor[initialState] = nil

	pool := newStatesPool(initialState, ins.UB)

	for !pool.isEmpty() {

		S := pool.popUnchecked()

		if pool.isDominatedByStateInPool(S) {
			delete(predecessor, S)
			continue
		}

		if S.IsFinal() {
			bestObjVal = S.t
			bestState = S
			continue
		}

		// EXTENDED_STATES_COUNT++

		for i := 0; i <= M; i++ {

			if (i == 0 && S.w[0] > 0 && S.w[1] == -1) ||
				(i > 0 && S.w[i] != -1 && (S.w[i+1] == -1 || i+1 == M+1)) {

				nextS_w := make([]int, M+2)
				copy(nextS_w, S.w)

				elapsedTime1 := Δ[S.x][i]

				for idx := 1; idx <= M; idx++ {
					if nextS_w[idx] != -1 {
						nextS_w[idx] = max(0, nextS_w[idx]-elapsedTime1)
					}
				}

				elapsedTime2 := 0
				if i != 0 {
					elapsedTime2 += nextS_w[i]
				}
				elapsedTime2 += Δ[i][i+1]

				nextS_t := S.t + elapsedTime1 + elapsedTime2
				if nextS_t > bestObjVal {
					continue
				}

				if i > 0 {
					nextS_w[i] = -1
				} else {
					nextS_w[i]--
				}

				for idx := 1; idx <= M; idx++ {
					if nextS_w[idx] != -1 {
						nextS_w[idx] = max(0, nextS_w[idx]-elapsedTime2)
					}
				}

				if i+1 != M+1 {
					nextS_w[i+1] = P[i+1]
				} else {
					nextS_w[i+1]++
				}

				nextS := newState(nextS_t, i+1, nextS_w)

				pushSucceeded := pool.tryPush(nextS)
				if pushSucceeded {
					predecessor[nextS] = S
				}
			}
		}
	}

	elapsed := time.Since(start)

	// fmt.Println("DP1 stats :")
	// fmt.Println(EXTENDED_STATES_COUNT, "# extensions")
	// fmt.Println()

	return utils.SolveResult{
		BestObjVal:    bestObjVal,
		ExecutionTime: elapsed,
		OptimalSol:    utils.BuildOptimalSol(bestState, predecessor),
	}
}
