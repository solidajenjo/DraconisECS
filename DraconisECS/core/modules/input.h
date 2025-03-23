#pragma once
#include "Module.h"

namespace module
{
class Input : public module::Module
{
public:
	bool update() override;
};
}  // namespace module
