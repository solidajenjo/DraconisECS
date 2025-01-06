#pragma once
namespace module
{
    class Module {
    public:
        virtual ~Module() = default;
        virtual bool init() { return true; };
		virtual bool preUpdate() { return true; };
        virtual bool update() { return true; };
		virtual bool postUpdate() { return true; };
        virtual bool shutdown() { return true; };
    };
} // namespace module