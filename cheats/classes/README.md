classes: contain "types" where the code in hooks will run.

pseudo with wrong code below
```c++
void renderBox(...); // something that draws it

type paintHook(...)
{
    ...

	if(panel == ...)
		renderBox(...); // WRONG, we don't want to manually pass them everytime

    ...
}
```

correct code
```c++
class RenderPlayer : public RenderableSurfaceType
{
public:
	constexpr RenderPlayer() :
		PaintBaseType{} // adding the object to the buffer with what to run
	{}

	virtual void init() override; // optonal to declare it here and later define
	virtual void draw() override; // put all stuff that draws anything related with "player render" keyword
private:
	void renderBox(...);
}

[[maybe_unused]] inline auto g_RenderPlayer = RenderPlayer{}; // we have our draw running without modyfying code in hooks
```
There are also types that don't run anything, look ``OnlyInitType``

Hooks body code is not modified due to the fact runAll() methods are static, as well as any base method ending with ``All()``