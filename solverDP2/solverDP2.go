package solverDP2

import (
	"slices"
	"time"

	"github.com/nemolino/AGV-Scheduling/instance"
	"github.com/nemolino/AGV-Scheduling/utils"
)

var J int // package global
var M int // package global

// var MAX_STATES_COUNT_IN_POOL int

func Solve(ins *instance.Instance, t utils.InstanceType) utils.SolveResult {

	J, M = ins.J, ins.M
	P, Δ := ins.P, ins.Δ

	// EXTENDED_STATES_COUNT := 0
	// MAX_STATES_COUNT_IN_POOL = 0

	start := time.Now()

	var bestObjVal int = ins.UB
	var bestState *state = nil

	initialState := newState(0, 0, make([]int, J), make([]int, J), J, 0)

	predecessor := make(map[utils.StateI]utils.StateI)
	predecessor[initialState] = nil

	pool := newStatesPool(initialState, ins.UB, utils.InsToPoolSize[t])

	for !pool.isEmpty() {

		S := pool.popUnchecked()

		if S.IsFinal() {
			if S.t < bestObjVal {
				bestObjVal = S.t
				bestState = S
			}
			continue
		}

		// EXTENDED_STATES_COUNT++

		for job := J - 1 - S.countFinished; job >= max(0, S.countNotStarted-1); job-- {

			//	grazie a job := J - 1 - S.countFinished :
			//		non mi muovo mai verso il serbatoio finale, cioè ho sempre dest != M+1
			// 	grazie a job >= max(0, S.countNotStarted-1)
			// 		mi muovo verso il serbatoio iniziale al massimo 1 volta, cioè ho dest == 0 al massimo 1 volta

			dest := S.xk[job]

			// mi muovo solo verso macchine con la dx libera o con a dx il serbatoio finale
			if dest+1 != M+1 && slices.Contains(S.xk, dest+1) {
				continue
			}

			nextS_t := S.t + Δ[S.x][dest] // passa il tempo di spostamento

			// prima di muovere il job bisogna aspettare la sua lavorazione finisca
			nextS_t = max(nextS_t, S.ek[job])

			// muovo il job verso la macchina successiva
			nextS_t += Δ[dest][dest+1] // passa il tempo di spostamento
			nextS_x := dest + 1

			nextS_xk := make([]int, J)
			copy(nextS_xk, S.xk)
			nextS_xk[job] += 1

			nextS_ek := make([]int, J)
			copy(nextS_ek, S.ek)
			if nextS_x == M+1 {
				nextS_ek[job] = -1
			} else {
				completionTime := nextS_t + P[nextS_x]
				nextS_ek[job] = completionTime
			}

			nextS_countNotStarted := S.countNotStarted
			if nextS_x == 1 {
				nextS_countNotStarted--
			}
			nextS_countFinished := S.countFinished
			if nextS_x == M+1 {
				nextS_countFinished++
			}

			for j := 0; j < job; j++ {
				nextS_ek[j] = max(nextS_ek[j], nextS_t+Δ[nextS_x][nextS_xk[j]])
			}
			for j := job + 1; j < J-nextS_countFinished; j++ {
				nextS_ek[j] = max(nextS_ek[j], nextS_t+Δ[nextS_x][nextS_xk[j]])
			}

			nextS := newState(nextS_t, nextS_x, nextS_xk, nextS_ek, nextS_countNotStarted, nextS_countFinished)

			pushSucceeded := pool.tryPush(nextS)
			if pushSucceeded {
				predecessor[nextS] = S
			}
		}
	}

	elapsed := time.Since(start)

	// fmt.Println("DP2 stats :")
	// fmt.Println(EXTENDED_STATES_COUNT, "# extensions")
	// fmt.Println(MAX_STATES_COUNT_IN_POOL, "max # states in pool")
	// fmt.Println()

	return utils.SolveResult{
		BestObjVal:    bestObjVal,
		ExecutionTime: elapsed,
		OptimalSol:    utils.BuildOptimalSol(bestState, predecessor),
	}
}
