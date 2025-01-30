package solverDP2

import "fmt"

type state struct {
	t  int
	x  int
	xk []int
	ek []int

	countNotStarted int
	countFinished   int
	indexInTimeSlot int
	id              uint
}

func newState(t int, x int, xk []int, ek []int, countNotStarted int, countFinished int) *state {
	return &state{t, x, xk, ek, countNotStarted, countFinished, -1, 0}
}

func (s *state) IsFinal() bool {
	return s.countFinished == J
}

func (s *state) dominatesAligned(s2 *state) bool {

	// REQUIRES : i job sono perfettamente allineati

	checkFrom := max(0, s.countNotStarted-1)
	checkTo := J - s.countFinished
	for k := checkFrom; k < checkTo; k++ {
		if s.ek[k] > s2.ek[k] {
			return false
		}
	}
	return true
}

func (s *state) dominatesNotAligned(s2 *state) bool {

	// REQUIRES : i job non sono perfettamente allineati

	if s.t > s2.t {
		return false
	}
	checkFrom := max(0, s.countNotStarted-1)
	checkTo := J - s.countFinished
	for k := checkFrom; k < checkTo; k++ {
		if s.xk[k] == s2.xk[k] && s.ek[k] > s2.ek[k] {
			return false
		}
	}
	return true
}

func (s *state) String() string {

	var xkString string = "[ "
	for _, x := range s.xk {
		xkString += fmt.Sprintf("%2d ", x)
	}
	xkString += "]"

	var ekString string = "[ "
	for _, x := range s.ek {
		ekString += fmt.Sprintf("%5d ", x)
	}
	ekString += "]"

	return fmt.Sprintf("State { t = %6d , x = %3d , xk = %v , ek = %v }", s.t, s.x, xkString, ekString)
}

// func (s *state) StringExtended() string {
// 	return fmt.Sprintf("State { t = %6d , x = %3d , p = %v , E = %v , # not started = %2d , # finished = %2d , indexInTimeSlot = %d , id = %v }",
// 		s.t, s.x, s.p, s.e, s.countNotStarted, s.countFinished, s.indexInTimeSlot, s.id)
// }
