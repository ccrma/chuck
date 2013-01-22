// recursive pile with fake subclass ( fail-correct )

class Pile { 

	class Item { 
		int foo;
	}

	Item stuff[];
}

Pile p;
<Pile.Item> i;
<Heap.Item> f;
<<<"success">>>;