#include "stdafx.h"
#include "B-Tree.h"

Node* createNode(void);
void deleteNode(Node* n);
size_t searchPosInNode(Node* n, int e);
Node* insertOrdered(Node* n, int e);
Node* splitNodeAndAddElement(Node* n, int e, size_t pos, int * median);
Node* findElementInNode(Node* n, int e, bool * success);
Node* addElementToNode(Node* n, int e);

Node* createNode(void) {
	int i = 0;
	Node* n = (Node*)malloc(sizeof(Node));
	if (n == NULL) {
		perror("Malloc failed");
		assert(n);
		exit(EXIT_FAILURE);
	}

	n->parent = NULL;
	n->size = 0;
	n->elements = calloc(MAXNODE, sizeof n->elements);
	if (n->elements == NULL) {
		perror("Malloc failed");
		assert(n->elements);
		exit(EXIT_FAILURE);
	}
	n->kids = calloc(MAXNODE + 1, sizeof(Node*));
	if (n->kids == NULL) {
		perror("Malloc failed");
		assert(n->kids);
		exit(EXIT_FAILURE);
	}

	//while (i < MAXNODE)
	//{
	//	n->elements[i] = 0;
	//	i++;
	//}
	//while (i > -1)
	//{
	//	n->kids[i] = NULL;
	//	i--;
	//}

	return n;
}

void deleteNode(Node* n) {
	if (n == NULL) return;

	for (size_t i = 0; i <= MAXNODE; i++) deleteNode(n->kids[i]);

	free(n->elements);
	free(n->kids);
	free(n);

	return;
}

size_t searchPosInNode(Node* n, int e) {
	int low = -1, high = n->size, mid;
	/* invariant: a[lo] < key <= a[hi] */

	while (low + 1 < high)
	{
		mid = (low + high) / 2;
		if (n->elements[mid] == e) {
			return mid;
		}
		else {
			if (n->elements[mid] < e) {
				low = mid;
			}
			else {
				high = mid;
			}
		}
	}

	return high;
}

Node* insertOrdered(Node* n, int e) {
	// Plain insert:
	//n->number_of_elements++;
	//n->elements[n->number_of_elements - 1] = e;

	// Ordered insert
	int i = n->size - 1;
	while (i >= 0 && n->elements[i] > e) {
		n->elements[i + 1] = n->elements[i];
		n->kids[i + 2] = n->kids[i + 1];
		i--;
	}
	n->elements[i + 1] = e;
	n->size++;

	return n;
}

Node* splitNodeAndAddElement(Node* n, int e, size_t pos, int * median) {
	Node* z = createNode();

	if (pos == ORDER) {
		// e goes to upper node
		// copy greater half to new right node
		*median = e;
		for (size_t i = ORDER; i < MAXNODE; i++) {
			addElementToNode(z, n->elements[i]);
			z->kids[i - ORDER] = n->kids[i];
		}
		n->size = ORDER;
	}
	if (pos < ORDER) {
		// e goes to left child
		// elements[ORDER - 1] goes to upper node
		*median = n->elements[ORDER - 1];
		for (size_t i = ORDER; i < MAXNODE; i++) {
			addElementToNode(z, n->elements[i]);
			z->kids[i - ORDER] = n->kids[i];
		}
		n->size = ORDER - 1;
		addElementToNode(n, e);
	}
	if (pos > ORDER) {
		// e goes to right child
		// elements[ORDER] goes to upper node
		*median = n->elements[ORDER];
		for (size_t i = ORDER + 1; i < MAXNODE; i++) {
			addElementToNode(z, n->elements[i]);
			z->kids[i - (ORDER + 1)] = n->kids[i];
		}
		addElementToNode(z, e);
		n->size = ORDER;
	}

	return z;
}

Node* findElementInNode(Node* n, int e, bool * success) {
	*success = false;
	size_t pos = searchPosInNode(n, e);

	if (pos < n->size && n->elements[pos] == e) {
		*success = true;
		return n;
	}
	if (n->kids[pos]) {
		return findElementInNode(n->kids[pos], e, success);
	}

	//Node* p = tree;
	//Node* curr = NULL;
	//while (!*success && (p != NULL)) {
	//	curr = p;
	//	for (size_t i = 0; i < p->size; i++) {
	//		if (e == p->elements[i]) {
	//			*success = true;
	//			break;
	//		}
	//		if (e < p->elements[i]) {
	//			p = p->kids[i];
	//			break;
	//		}
	//		if (i == p->size - 1) {
	//			p = p->kids[i + 1];
	//			break;
	//		}
	//	}
	//}

	return n;
}

Node* addElementToNode(Node* n, int e) {
	int median;
	Node* z;
	size_t pos = searchPosInNode(n, e);

	if (n->size >= MAXNODE)
	{
		// split node
		perror("Node is full");
		z = splitNodeAndAddElement(n, e, pos, &median);

		if (n->parent == NULL) {
			// Create new root
			n->parent = createNode();
			n->parent->kids[0] = n;
		}
		addElementToNode(n->parent, median);
		pos = searchPosInNode(n->parent, median);
		n->parent->kids[pos + 1] = z;
		z->parent = n->parent;
	}
	else {
		insertOrdered(n, e);
	}

	return n;
}

Node* addElementToTree(Node* tree, int e) {
	// Ensure to use the root node.
	while (tree->parent != NULL) tree = tree->parent;

	// Ensure element isn't already in the tree
	bool success;
	Node* tmp = findElementInNode(tree, e, &success);
	if (success) return tree;

	addElementToNode(tmp, e);

	// Ensure to return the root node.
	while (tree->parent != NULL) tree = tree->parent;
	return tree;
}

Node* createBTree(void) {
	return createNode();
}

void deleteBTree(Node* tree) {
	deleteNode(tree);
}

//btree t = NULL;
//node_pointer current_node = NULL;
//
//node_pointer addElementToNode(node_pointer n, stdelement e) {
//	//n->number_of_elements++;
//	//n->elements[n->number_of_elements - 1] = e;
//
//	int i = 0, k = n->number_of_elements;
//	while (e > n->elements[i])
//	{
//		i++;
//	}
//	while (k > i)
//	{
//		n->elements[k] = n->elements[k - 1];
//		k--;
//	}
//	n->elements[k] = e;
//	n->number_of_elements++;
//
//	return n;
//}
//
//node_pointer delElementFromNode(node_pointer n, stdelement e) {
//	return n;
//}
//
//node_pointer createNode(void) {
//	int i = 0;
//	node_pointer tmp = (node_pointer)malloc(sizeof(struct node));
//	assert(tmp);
//
//	tmp->number_of_elements = 0;
//	while (i < MAXNODE)
//	{
//		tmp->elements[i] = 0;
//		i++;
//	}
//	while (i > -1)
//	{
//		tmp->children[i] = NULL;
//		i--;
//	}
//
//	return tmp;
//}
//
//node_pointer findElementInTree(btree t, stdelement e, bool * success) {
//	*success = false;
//	node_pointer p = t->root;
//	node_pointer curr = NULL;
//
//	while (!*success && (p != NULL)) {
//		curr = p;
//
//		for (int i = 0; i < p->number_of_elements; i++) {
//			if (e == p->elements[i]) {
//				*success = true;
//				break;
//			}
//			if (e < p->elements[i]) {
//				p = p->children[i];
//				break;
//			}
//			if (i == p->number_of_elements - 1) {
//				p = p->children[i + 1];
//				break;
//			}
//		}
//	}
//
//	return curr;
//}
//
//node_pointer splitNode(node_pointer n) {
//	//stdelement tmpElems[MAXNODE + 1] = calloc(MAXNODE + 1, sizeof(stdelement));
//	//assert(tmpElems);
//	node_pointer z = createNode();
//	int i = ORDER;
//	while (i < MAXNODE)
//	{
//		addElementToNode(z, n->elements[i]);
//		z->children[i - ORDER] = n->children[i];
//		n->elements[i] = 0;
//		n->number_of_elements--;
//		i++;
//	}
//
//	return z;
//}
//
//int searchPos(int n, stdelement * arr, stdelement e) {
//	int low = -1, high = n, mid;
//	/* invariant: a[lo] < key <= a[hi] */
//
//	while (low + 1 < high)
//	{
//		mid = (low + high) / 2;
//		if (arr[mid] == e) {
//			return mid;
//		}
//		else {
//			if (arr[mid] < e) {
//				low = mid;
//			}
//			else {
//				high = mid;
//			}
//		}
//	}
//
//	return high;
//}
//
//btree insertElementIntoTree(btree t, stdelement e) {
//	bool success = false;
//	node_pointer tmp = findElementInTree(t, e, &success);
//
//	if (success) {
//		// element allready in tree;
//		return t;
//	}
//
//	if (tmp == NULL) {
//		t->root = addElementToNode(createNode(), e);
//	}
//	else {
//		if (tmp->number_of_elements < MAXNODE) {
//			addElementToNode(tmp, e);
//		}
//		else {
//			// TODO
//			// splitNode
//			int pos = searchPos(tmp->number_of_elements, tmp->elements, e);
//
//			if (pos == ORDER) {
//				node_pointer x = addElementToNode(createNode(), e);
//				x->children[1] = splitNode(tmp);
//				x->children[0] = tmp;
//				t->root = x;
//			}
//		}
//	}
//
//	return t;
//}
//
//btree createBTree(stdelement e) {
//	btree tmp = (btree)calloc(1, sizeof(btree));
//	assert(tmp);
//
//	tmp->root = addElementToNode(createNode(), e);
//
//	return tmp;
//}
