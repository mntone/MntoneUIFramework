#pragma once

#pragma warning( disable: 4996 )

namespace mnfx {

class os_version final
{
public:
	enum os_label { unknown = 0, before = 49, two_thousand = 50, xp, vista = 60, seven, eight, eight_one, ten, later };

	os_version() noexcept
	{
		OSVERSIONINFOW os_version = { sizeof(OSVERSIONINFOW) };
		GetVersionExW(&os_version);

		if (os_version.dwMajorVersion == 10)
		{
			switch (os_version.dwMinorVersion)
			{
			case 0:
				os_version_label_ = os_label::ten;
				break;
			default:
				os_version_label_ = os_label::unknown;
				break;
			}
		}
		else if (os_version.dwMajorVersion > 6)
		{
			os_version_label_ = os_label::unknown;
		}
		else if (os_version.dwMajorVersion == 6)
		{
			switch (os_version.dwMinorVersion)
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
			case 0:
				os_version_label_ = os_label::vista;
				break;
			default:
				os_version_label_ = os_label::unknown;
				break;
			}
		}
		else if (os_version.dwMajorVersion == 5)
		{
			switch (os_version.dwMinorVersion)
			{
			case 1:
				os_version_label_ = os_label::xp;
				break;
			case 0:
				os_version_label_ = os_label::two_thousand;
				break;
			default:
				os_version_label_ = os_label::unknown;
				break;
			}
		}
		else
		{
			os_version_label_ = os_label::before;
		}

		workstation_ = os_version.dwPlatformId == VER_NT_WORKSTATION;
	}

	bool operator==(os_label const& rhs) const noexcept { return os_version_label_ == rhs; }
	bool operator!=(os_label const& rhs) const noexcept { return os_version_label_ != rhs; }
	bool operator>(os_label const& rhs) const noexcept { return os_version_label_ > rhs; }
	bool operator<(os_label const& rhs) const noexcept { return os_version_label_ < rhs; }
	bool operator>=(os_label const& rhs) const noexcept { return os_version_label_ >= rhs; }
	bool operator<=(os_label const& rhs) const noexcept { return os_version_label_ <= rhs; }

	bool operator==(os_version const& rhs) const noexcept { return os_version_label_ == rhs.os_version_label_; }
	bool operator!=(os_version const& rhs) const noexcept { return os_version_label_ != rhs.os_version_label_; }
	bool operator>(os_version const& rhs) const noexcept { return os_version_label_ > rhs.os_version_label_; }
	bool operator<(os_version const& rhs) const noexcept { return os_version_label_ < rhs.os_version_label_; }
	bool operator>=(os_version const& rhs) const noexcept { return os_version_label_ >= rhs.os_version_label_; }
	bool operator<=(os_version const& rhs) const noexcept { return os_version_label_ <= rhs.os_version_label_; }

	os_label get() const noexcept { return os_version_label_; }
	bool workstation() const noexcept { return workstation_; }

private:
	os_label os_version_label_;
	bool workstation_;
};

}