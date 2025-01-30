package solverDP1

import (
	"fmt"
)

type state struct {
	t int
	x int
	w []int

	key string
}

// l is w[0]
// u is w[M+1]

func newState(t int, x int, w []int) *state {
	return &state{t, x, w, ""}
}

func (s *state) IsFinal() bool {
	return s.x == M+1 && s.w[s.x] == J
}

func (s *state) initKey() {
	s.key = fmt.Sprintf("%d %v", s.x, s.w)
}

func (s *state) String() string {

	var wString string = "[ "
	for _, x := range s.w[1 : M+1] {
		if x == -1 {
			wString += "  ⊥ "
		} else {
			wString += fmt.Sprintf("%3d ", x)
		}
	}
	wString += "]"

	return fmt.Sprintf("State { t = %6d , x = %3d , l = %3d , w = %s , u = %3d }",
		s.t, s.x, s.w[0], wString, s.w[M+1])
}
