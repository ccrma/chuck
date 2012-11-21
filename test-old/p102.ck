// recursive pile with fake subclass ( fail-correct )

class Pile { 

	class Item { 
		int foo;
	}

	Item stuff[];
}

Pile p;
<Pile.Item> i;
<Pile.Fake> f;
<<<"success">>>;