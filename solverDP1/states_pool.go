package solverDP1

type statesPool struct {
	timeSlots    [][](*state)
	curSlot      int
	curSlotIndex int
	h            map[string]int
}

func newStatesPool(initialState *state, UB int) *statesPool {

	timeSlots := make([][](*state), UB+1)
	timeSlots[0] = append(timeSlots[0], initialState)
	curSlot := 0
	curSlotIndex := 0

	h := make(map[string]int)
	initialState.initKey()
	h[initialState.key] = initialState.t

	return &statesPool{timeSlots, curSlot, curSlotIndex, h}
}

func (sp *statesPool) isEmpty() bool {
	if sp.curSlotIndex < len(sp.timeSlots[sp.curSlot]) {
		return false
	}
	sp.curSlot++
	sp.curSlotIndex = 0
	for sp.curSlot < len(sp.timeSlots) {
		if sp.curSlotIndex < len(sp.timeSlots[sp.curSlot]) {
			return false
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
	return s
}

func (sp *statesPool) tryPush(s *state) bool {
	s.initKey()
	if value, ok := sp.h[s.key]; ok && value <= s.t {
		return false
	}
	sp.h[s.key] = s.t
	sp.timeSlots[s.t] = append(sp.timeSlots[s.t], s)
	return true
}

func (sp *statesPool) isDominatedByStateInPool(s *state) bool {
	return sp.h[s.key] < s.t
}
