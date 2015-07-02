#pragma once

#pragma warning( disable: 4996 )

namespace mnfx {

class os_version
{
public:
	enum os_label { before = 60, seven, eight, eight_one, later };

	os_version()
	{
		OSVERSIONINFOW osVersion = { sizeof(OSVERSIONINFOW) };
		GetVersionExW(&osVersion);

		if (osVersion.dwMajorVersion > 6)
		{
			os_version_label_ = os_label::later;
		}
		else
		{
			switch (osVersion.dwMinorVersion)
			{
			case 3:
				os_version_label_ = os_label::eight_one;
				break;
			case 2:
				os_version_label_ = os_label::eight;
				break;
			case 1:
				os_version_label_ = os_label::seven;
				break;
			default:
				os_version_label_ = os_label::before;
				break;
			}
		}
	}

	bool operator==(os_label const& rhs) const noexcept { return this->os_version_label_ == rhs; }
	bool operator!=(os_label const& rhs) const noexcept { return this->os_version_label_ != rhs; }
	bool operator>(os_label const& rhs) const noexcept { return this->os_version_label_ > rhs; }
	bool operator<(os_label const& rhs) const noexcept { return this->os_version_label_ < rhs; }
	bool operator>=(os_label const& rhs) const noexcept { return this->os_version_label_ >= rhs; }
	bool operator<=(os_label const& rhs) const noexcept { return this->os_version_label_ <= rhs; }

	bool operator==(os_version const& rhs) const noexcept { return this->os_version_label_ == rhs.os_version_label_; }
	bool operator!=(os_version const& rhs) const noexcept { return this->os_version_label_ != rhs.os_version_label_; }
	bool operator>(os_version const& rhs) const noexcept { return this->os_version_label_ > rhs.os_version_label_; }
	bool operator<(os_version const& rhs) const noexcept { return this->os_version_label_ < rhs.os_version_label_; }
	bool operator>=(os_version const& rhs) const noexcept { return this->os_version_label_ >= rhs.os_version_label_; }
	bool operator<=(os_version const& rhs) const noexcept { return this->os_version_label_ <= rhs.os_version_label_; }

	os_label get() const { return os_version_label_; }

private:
	os_label os_version_label_;
};

}