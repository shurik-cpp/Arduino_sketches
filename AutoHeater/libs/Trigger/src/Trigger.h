#pragma once

class Trigger {
public:
	Trigger() = default;
	~Trigger() = default;

	bool IsRisingFront(const bool variable);
	bool IsFallingFront(const bool variable);

private:
	short r_trig = 0;
	short f_trig = 2;
};

