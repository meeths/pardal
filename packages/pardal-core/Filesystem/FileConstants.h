#pragma once
namespace pdl
{

namespace FileConstants
{
enum OpenMode
{	
	Read			= 0x01,
	Write			= 0x02,
	Binary			= 0x04,
	AtEnd			= 0x08,
	Append			= 0x0f,
	Truncate		= 0x10
};

enum class PositionOrigin : int
{
	Start,
	End,
	Current
};
};

}