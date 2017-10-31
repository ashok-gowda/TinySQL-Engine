class statements
{
private:
	bool correctSyntactically;
public:
	virtual void parse() = 0;
	virtual void checkSyntax() = 0;
};