#pragma once

#include <atomic>

namespace nitro_compat
{
class Hook
{
public:
    virtual ~Hook() = default;
    virtual void UnsetCallbackAndDisableHook() = 0;
    virtual uint32_t GetTrampolineAddress() const = 0;
    virtual uint32_t GetOriginalFunctionAddress() const = 0;
    virtual bool IsHookActive() const = 0;
};

template<class... Args> class Next
{
public:
    virtual void Invoke(Args...) = 0;
};
template<class... Args> class Handler
{
public:
    virtual void Handle(Args..., Next<Args...>*) = 0;
};
template<class... Args> class Chain
{
public:
    virtual ~Chain() = default;
    virtual void DetachHook() = 0;
    virtual bool IsHookAttached() = 0;
    virtual uint32_t GetSubscribersCount() const = 0;
    virtual uint32_t GetHandlersCount() const = 0;
    virtual void AttachHook(void*) = 0;
    virtual Hook* GetHook() const = 0;
    virtual bool AddHandler(Handler<Args...>*) = 0;
    virtual bool RemoveHandler(Handler<Args...>*) = 0;
    virtual bool Subscribe(void*) = 0;
    virtual bool Unsubscribe(void*) = 0;
};

template<class Result, class... Args> class ResultNext
{
public:
	virtual Result Invoke(Args...) = 0;
};
template<class Result, class... Args> class ResultHandler
{
public:
	virtual Result Handle(Args..., ResultNext<Result, Args...>*) = 0;
};
template<class Result, class... Args> class ResultChain
{
public:
	virtual ~ResultChain() = default;
	virtual void DetachHook() = 0;
	virtual bool IsHookAttached() = 0;
	virtual uint32_t GetSubscribersCount() const = 0;
	virtual uint32_t GetHandlersCount() const = 0;
	virtual void AttachHook(void*) = 0;
	virtual Hook* GetHook() const = 0;
	virtual bool AddHandler(ResultHandler<Result, Args...>*) = 0;
	virtual bool RemoveHandler(ResultHandler<Result, Args...>*) = 0;
	virtual bool Subscribe(void*) = 0;
	virtual bool Unsubscribe(void*) = 0;
};

struct Function
{
    void* chain = nullptr;
    Hook* hook = nullptr;
};
static_assert(sizeof(Function) == 8, "Nitro adapter supports Win32 only");

enum class FunctionId { Frame, ForceCVars, DynamicSound, SpriteSet };

bool run_on_engine_thread(bool (*operation)(void*), void* context);
void pump_pending_operation();

class Compatibility
{
public:
    static Compatibility& get() { static Compatibility instance; return instance; }
    bool initialize(const wchar_t* engine, int build);
    bool enabled() const { return m_enabled; }
    int* host_initialized() const { return m_host_initialized; }
    Function function(FunctionId id) const { return m_functions[static_cast<size_t>(id)]; }
	Function crosshair_function() const { return m_crosshair; }
private:
    bool m_enabled = false;
    int* m_host_initialized = nullptr;
    Function m_functions[4]{};
	Function m_crosshair{};
};

template<class... Args> class Binding final : public Handler<Args...>
{
public:
    bool attach(Function fn, void (*callback)(Args...))
    {
		if (m_attached || !fn.chain || !fn.hook)
		{
			CConsole::the().error("Nitro binding: invalid or duplicate handler attachment.");
			return false;
		}
        m_tls = TlsAlloc();
		if (m_tls == TLS_OUT_OF_INDEXES)
		{
			CConsole::the().error("Nitro binding: TlsAlloc failed ({}).", GetLastError());
			return false;
		}
        m_chain = static_cast<Chain<Args...>*>(fn.chain);
        m_callback = callback;
        m_attached = run_on_engine_thread([](void* context) {
            auto self = static_cast<Binding*>(context);
            return self->m_chain->AddHandler(self);
        }, this);
        if (!m_attached) { TlsFree(m_tls); m_tls = TLS_OUT_OF_INDEXES; }
        return m_attached;
    }

    bool detach()
    {
        if (!m_attached) return true;
        if (!run_on_engine_thread([](void* context) {
            auto self = static_cast<Binding*>(context);
            return self->m_chain->RemoveHandler(self);
        }, this)) return false;
        m_attached = false;
        return true;
    }

    bool attached() const { return m_attached; }
    void drain()
    {
        while (m_invocations.load(std::memory_order_acquire)) Sleep(1);
        if (m_tls != TLS_OUT_OF_INDEXES) { TlsFree(m_tls); m_tls = TLS_OUT_OF_INDEXES; }
    }

    void Handle(Args... args, Next<Args...>* next) override
    {
        m_invocations.fetch_add(1, std::memory_order_acq_rel);
        struct InvocationScope
        {
            Binding* self;
            void* previous;
            ~InvocationScope()
            {
                TlsSetValue(self->m_tls, previous);
                self->m_invocations.fetch_sub(1, std::memory_order_release);
            }
        } scope{this, TlsGetValue(m_tls)};
        if (TlsSetValue(m_tls, next))
        {
            m_callback(args...);
        }
        else next->Invoke(args...);
    }

    void call_next(Args... args) const
    {
        auto next = static_cast<Next<Args...>*>(TlsGetValue(m_tls));
        if (next) next->Invoke(args...);
    }

private:
    Chain<Args...>* m_chain = nullptr;
    void (*m_callback)(Args...) = nullptr;
    DWORD m_tls = TLS_OUT_OF_INDEXES;
    bool m_attached = false;
    std::atomic<unsigned> m_invocations{0};
};

template<class Result, class... Args> class ResultBinding final : public ResultHandler<Result, Args...>
{
public:
	bool attach(Function fn, Result (*callback)(Args...))
	{
		if (m_attached || !fn.chain || !fn.hook) return false;
		m_tls = TlsAlloc();
		if (m_tls == TLS_OUT_OF_INDEXES) return false;
		m_chain = static_cast<ResultChain<Result, Args...>*>(fn.chain);
		m_callback = callback;
		m_attached = run_on_engine_thread([](void* context) {
			auto self = static_cast<ResultBinding*>(context);
			return self->m_chain->AddHandler(self);
		}, this);
		if (!m_attached) { TlsFree(m_tls); m_tls = TLS_OUT_OF_INDEXES; }
		return m_attached;
	}
	bool detach()
	{
		if (!m_attached) return true;
		if (!run_on_engine_thread([](void* context) {
			auto self = static_cast<ResultBinding*>(context);
			return self->m_chain->RemoveHandler(self);
		}, this)) return false;
		m_attached = false;
		return true;
	}
	void drain()
	{
		while (m_invocations.load(std::memory_order_acquire)) Sleep(1);
		if (m_tls != TLS_OUT_OF_INDEXES) { TlsFree(m_tls); m_tls = TLS_OUT_OF_INDEXES; }
	}
	Result Handle(Args... args, ResultNext<Result, Args...>* next) override
	{
		m_invocations.fetch_add(1, std::memory_order_acq_rel);
		struct InvocationScope
		{
			ResultBinding* self;
			void* previous;
			~InvocationScope()
			{
				TlsSetValue(self->m_tls, previous);
				self->m_invocations.fetch_sub(1, std::memory_order_release);
			}
		} scope{this, TlsGetValue(m_tls)};
		if (TlsSetValue(m_tls, next)) return m_callback(args...);
		return next->Invoke(args...);
	}
	Result call_next(Args... args) const
	{
		auto next = static_cast<ResultNext<Result, Args...>*>(TlsGetValue(m_tls));
		return next ? next->Invoke(args...) : Result{};
	}
private:
	ResultChain<Result, Args...>* m_chain = nullptr;
	Result (*m_callback)(Args...) = nullptr;
	DWORD m_tls = TLS_OUT_OF_INDEXES;
	bool m_attached = false;
	std::atomic<unsigned> m_invocations{0};
};
}
