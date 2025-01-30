package solverDP2

import (
	"log"

	"github.com/bits-and-blooms/bitset"
)

type statesPool struct {
	timeSlots    [][](*state)
	curSlot      int
	curSlotIndex int

	ON  [][]*bitset.BitSet
	GEQ [][]*bitset.BitSet
	LEQ [][]*bitset.BitSet

	idToState   [](*state)
	idsPoolSize uint
	idsPool     []uint
	idsPoolTop  int

	idContainer        []uint
	idContainerAligned []uint

	candidateAligned              *bitset.BitSet
	candidateDominatorsNonAligned *bitset.BitSet
	candidateDominatedNonAligned  *bitset.BitSet
}

func newStatesPool(initialState *state, UB int, idsPoolSize uint) *statesPool {

	// --- creating all needed data structures

	timeSlots := make([][](*state), UB+1)
	curSlot := 0
	curSlotIndex := 0

	ON := make([][]*bitset.BitSet, J)
	for k := 0; k < J; k++ {
		ON[k] = make([]*bitset.BitSet, M+2)
		for i := 0; i <= M+1; i++ {
			ON[k][i] = bitset.MustNew(idsPoolSize)
		}
	}

	GEQ := make([][]*bitset.BitSet, J)
	for k := 0; k < J; k++ {
		GEQ[k] = make([]*bitset.BitSet, M+2)
		for i := 0; i <= M+1; i++ {
			GEQ[k][i] = bitset.MustNew(idsPoolSize)
		}
	}

	LEQ := make([][]*bitset.BitSet, J)
	for k := 0; k < J; k++ {
		LEQ[k] = make([]*bitset.BitSet, M+2)
		for i := 0; i <= M+1; i++ {
			LEQ[k][i] = bitset.MustNew(idsPoolSize)
		}
	}

	idToState := make([](*state), idsPoolSize)

	// new full stack of IDs
	idsPool := make([]uint, idsPoolSize)
	for i := uint(0); i < idsPoolSize; i++ {
		idsPool[i] = i
	}
	idsPoolTop := int(idsPoolSize) - 1

	idContainer := make([]uint, idsPoolSize)
	idContainerAligned := make([]uint, idsPoolSize)

	candidateAligned := bitset.MustNew(idsPoolSize)
	candidateDominatorsNonAligned := bitset.MustNew(idsPoolSize)
	candidateDominatedNonAligned := bitset.MustNew(idsPoolSize)

	// --- initial state handling

	timeSlots[0] = append(timeSlots[0], initialState)
	initialState.indexInTimeSlot = 0

	initialState.id = idsPool[idsPoolTop]
	idsPoolTop--
	idToState[initialState.id] = initialState

	// init ON, GEQ and LEQ
	for k := 0; k < J; k++ {
		kCurStation := initialState.xk[k]
		ON[k][kCurStation] = ON[k][kCurStation].Set(initialState.id)
		for i := 0; i <= kCurStation; i++ {
			GEQ[k][i].Set(initialState.id)
		}
		for i := kCurStation; i <= M+1; i++ {
			LEQ[k][i].Set(initialState.id)
		}
	}

	return &statesPool{
		timeSlots, curSlot, curSlotIndex,
		ON, GEQ, LEQ,
		idToState, idsPoolSize, idsPool, idsPoolTop,
		idContainer, idContainerAligned,
		candidateAligned, candidateDominatorsNonAligned, candidateDominatedNonAligned,
	}
}

func (sp *statesPool) isEmpty() bool {

	for sp.curSlot < len(sp.timeSlots) {
		for sp.curSlotIndex < len(sp.timeSlots[sp.curSlot]) {
			if sp.timeSlots[sp.curSlot][sp.curSlotIndex] != nil {
				return false
			}
			sp.curSlotIndex++
		}
		sp.curSlot++
		sp.curSlotIndex = 0
	}
	return true
}

func (sp *statesPool) popUnchecked() *state {

	// REQUIRES : sp.isEmpty() == false

	s := sp.timeSlots[sp.curSlot][sp.curSlotIndex]
	sp.curSlotIndex++

	for k := 0; k < J; k++ {
		kCurStation := s.xk[k]
		sp.ON[k][kCurStation].Clear(s.id)
		for i := 0; i <= kCurStation; i++ {
			sp.GEQ[k][i].Clear(s.id)
		}
		for i := kCurStation; i <= M+1; i++ {
			sp.LEQ[k][i].Clear(s.id)
		}
	}

	sp.idToState[s.id] = nil
	sp.idsPoolTop++
	sp.idsPool[sp.idsPoolTop] = s.id

	return s
}

func (sp *statesPool) tryPush(s *state) bool {

	// countStatesInPool := 0
	// for m := 0; m <= M+1; m++ {
	// 	countStatesInPool += int(sp.ON[0][m].Count())
	// }
	// MAX_STATES_COUNT_IN_POOL = max(MAX_STATES_COUNT_IN_POOL, countStatesInPool)

	// ------ checking if some state in the pool dominates s

	// --- exact alignment check
	sp.candidateAligned.ClearAll()
	sp.candidateAligned.InPlaceUnion(sp.ON[0][s.xk[0]])
	for k := 1; k < J; k++ {
		sp.candidateAligned.InPlaceIntersection(sp.ON[k][s.xk[k]])
	}

	if sp.candidateAligned.Any() {
		sp.idContainerAligned = sp.idContainerAligned[:sp.candidateAligned.Count()]
		sp.candidateAligned.NextSetMany(0, sp.idContainerAligned)
		for _, dominatorID := range sp.idContainerAligned {
			dominator := sp.idToState[dominatorID]
			if dominator.dominatesAligned(s) {
				// DOMINANCE_CHECK_SUCCEEDED_IMMEDIATELY_ALIGNED++
				return false
			}
		}
	}

	// --- non-exact alignment check

	sp.candidateDominatorsNonAligned.ClearAll()

	if s.countFinished != 0 {

		sp.candidateDominatorsNonAligned.InPlaceUnion(sp.ON[J-s.countFinished][s.xk[J-s.countFinished]])
		if sp.candidateDominatorsNonAligned.Any() {
			for k := J - s.countFinished - 1; k >= s.countNotStarted; k-- {
				kCurStation := s.xk[k]
				sp.candidateDominatorsNonAligned.InPlaceIntersection(sp.GEQ[k][kCurStation])
				if sp.candidateDominatorsNonAligned.None() {
					break
				}
			}
		}

	} else {

		k := J - s.countFinished - 1
		kCurStation := s.xk[k]
		sp.candidateDominatorsNonAligned.InPlaceUnion(sp.GEQ[k][kCurStation])
		if sp.candidateDominatorsNonAligned.Any() {
			for k := J - s.countFinished - 2; k >= s.countNotStarted; k-- {
				kCurStation := s.xk[k]
				sp.candidateDominatorsNonAligned.InPlaceIntersection(sp.GEQ[k][kCurStation])
				if sp.candidateDominatorsNonAligned.None() {
					break
				}
			}
		}

	}

	sp.candidateDominatorsNonAligned.InPlaceDifference(sp.candidateAligned)

	if sp.candidateDominatorsNonAligned.Any() {
		sp.idContainer = sp.idContainer[:sp.candidateDominatorsNonAligned.Count()]
		sp.candidateDominatorsNonAligned.NextSetMany(0, sp.idContainer)
		for _, dominatorID := range sp.idContainer {
			dominator := sp.idToState[dominatorID]
			if dominator.dominatesNotAligned(s) {
				// DOMINANCE_CHECK_SUCCEEDED_IMMEDIATELY_NON_ALIGNED++
				return false
			}
		}
	}

	// ------ checking if s dominates some state in the pool

	// --- exact alignment check

	if sp.candidateAligned.Any() {
		for _, dominatedID := range sp.idContainerAligned {
			dominated := sp.idToState[dominatedID]
			if s.dominatesAligned(dominated) {
				sp.timeSlots[dominated.t][dominated.indexInTimeSlot] = nil
				for k := 0; k < J; k++ {
					kCurStation := dominated.xk[k]
					sp.ON[k][kCurStation].Clear(dominatedID)
					for i := 0; i <= kCurStation; i++ {
						sp.GEQ[k][i].Clear(dominatedID)
					}
					for i := kCurStation; i <= M+1; i++ {
						sp.LEQ[k][i].Clear(dominatedID)
					}
				}
				sp.idToState[dominatedID] = nil
				sp.idsPoolTop++
				sp.idsPool[sp.idsPoolTop] = dominatedID
				// ELIMINATED_FROM_POOL_ALIGNED++
			}
		}
	}

	// --- non-exact alignment check

	sp.candidateDominatedNonAligned.ClearAll()

	if !s.IsFinal() {

		// s.countFinished < J
		if s.countNotStarted != 0 {

			sp.candidateDominatedNonAligned.InPlaceUnion(sp.ON[s.countNotStarted-1][s.xk[s.countNotStarted-1]])
			if sp.candidateDominatedNonAligned.Any() {
				for k := s.countNotStarted; k <= J-s.countFinished-1; k++ {
					kCurStation := s.xk[k]
					sp.candidateDominatedNonAligned.InPlaceIntersection(sp.LEQ[k][kCurStation])
					if sp.candidateDominatedNonAligned.None() {
						break
					}
				}
			}

		} else {

			k := s.countNotStarted
			kCurStation := s.xk[k]
			for i := 0; i <= kCurStation; i++ {
				sp.candidateDominatedNonAligned.InPlaceUnion(sp.ON[k][i])
			}
			if sp.candidateDominatedNonAligned.Any() {
				for k := s.countNotStarted + 1; k <= J-s.countFinished-1; k++ {
					kCurStation := s.xk[k]
					sp.candidateDominatedNonAligned.InPlaceIntersection(sp.LEQ[k][kCurStation])
					if sp.candidateDominatedNonAligned.None() {
						break
					}
				}
			}

		}

	}

	sp.candidateDominatedNonAligned.InPlaceDifference(sp.candidateAligned)

	if sp.candidateDominatedNonAligned.Any() {

		sp.idContainer = sp.idContainer[:sp.candidateDominatedNonAligned.Count()]
		sp.candidateDominatedNonAligned.NextSetMany(0, sp.idContainer)
		for _, dominatedID := range sp.idContainer {
			dominated := sp.idToState[dominatedID]
			if s.dominatesNotAligned(dominated) {
				sp.timeSlots[dominated.t][dominated.indexInTimeSlot] = nil
				for k := 0; k < J; k++ {
					kCurStation := dominated.xk[k]
					sp.ON[k][dominated.xk[k]].Clear(dominatedID)
					for i := 0; i <= kCurStation; i++ {
						sp.GEQ[k][i].Clear(dominatedID)
					}
					for i := kCurStation; i <= M+1; i++ {
						sp.LEQ[k][i].Clear(dominatedID)
					}
				}
				sp.idToState[dominatedID] = nil
				sp.idsPoolTop++
				sp.idsPool[sp.idsPoolTop] = dominatedID
				// ELIMINATED_FROM_POOL_NON_ALIGNED++
			}
		}
	}

	sp.timeSlots[s.t] = append(sp.timeSlots[s.t], s)
	s.indexInTimeSlot = len(sp.timeSlots[s.t]) - 1

	if sp.idsPoolTop < 0 {
		log.Fatal("ERROR popAvailableId : not enough IDs, idsPoolSize must be increased")
	}
	s.id = sp.idsPool[sp.idsPoolTop]
	sp.idsPoolTop--
	sp.idToState[s.id] = s

	// update ON, GEQ and LEQ
	for k := 0; k < J; k++ {
		kCurStation := s.xk[k]
		sp.ON[k][kCurStation] = sp.ON[k][kCurStation].Set(s.id)
		for i := 0; i <= kCurStation; i++ {
			sp.GEQ[k][i].Set(s.id)
		}
		for i := kCurStation; i <= M+1; i++ {
			sp.LEQ[k][i].Set(s.id)
		}
	}

	// MAX_STATES_COUNT_IN_POOL = max(MAX_STATES_COUNT_IN_POOL, countStatesInPool+1)

	return true
}
