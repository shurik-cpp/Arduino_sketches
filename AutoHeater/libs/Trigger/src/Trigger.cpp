#include "Trigger.h"

bool Trigger::IsRisingFront(const bool variable) {
	if (!variable) {
		r_trig = 0;
	}
	else if (variable && r_trig == 0) {
		r_trig = 1;
	}

	if (r_trig == 1) {
		r_trig = 2;
		return true;
	}
	return false;
}

bool Trigger::IsFallingFront(const bool variable) {
	if (variable) {
		f_trig = 0;
	}
	else if (!variable && f_trig == 0) {
		f_trig = 1;
	}

	if (f_trig == 1) {
		f_trig = 2;
		return true;
	}
	return false;
}