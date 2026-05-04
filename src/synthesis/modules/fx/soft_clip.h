#include "fx.h"

namespace synthesis {
	class SoftClip : public Fx {
	public:
		SoftClip();
		void generate_buf();
		void set_drive(float_s value);

		enum BufType {
			AUDIO,
			DRIVE
		};

	protected:
		std::vector<const float_s*> in_bufs[2];

	private:
		float_s drive;
	};
}