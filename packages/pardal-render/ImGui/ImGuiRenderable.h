
#pragma once

// Created on 2026-01-14 by franciscom

namespace pdl
{

class ImGuiRenderable
{
public:
	virtual ~ImGuiRenderable() = default;
	virtual void ImGuiPreRender() {};
	virtual void ImGuiRender() = 0;
};

}

