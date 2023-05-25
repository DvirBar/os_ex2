#include <linux/kernel.h>
#include <linux/sched.h>

asmlinkage long sys_hello(void) {
	printk("Hello, World!\n");
	return 0;
}

asmlinkage int sys_set_weight(int weight) {
	if(weight < 0) {
		return -EINVAL;
	}
	
	current->process_weight = weight;
	return 0;
}

asmlinkage int sys_get_weight(void) {
	return current->process_weight;
}


void get_heaviest_descendant_aux(int* maxWeight, int* pidOfMax, struct task_struct* root);
void get_heaviest(int weight1, int pid1, int weight2, int pid2, int* maxWeight, int* pidOfMax);

asmlinkage pid_t sys_get_heaviest_descendant(void) {
	int maxWeight = 0;
	int pidOfMax = INT_MAX;
	get_heaviest_descendant_aux(&maxWeight, &pidOfMax, current);
	
	return pidOfMax;
}

void get_heaviest_descendant_aux(int* maxWeight, int* pidOfMax, struct task_struct* root) {
	struct task_struct *task;
	struct list_head *list;
	int maxChild = 0;
	int pidOfChild = INT_MAX;
	int subtreeMax = 0;
	int pidOfSubtree = INT_MAX;
	int hasChildren = 0;

	list_for_each(list, &root->children) {
		hasChildren = 1;
		task = list_entry(list, struct task_struct, sibling);
		get_heaviest_descendant_aux(&maxChild, &pidOfChild, task);
		get_heaviest(task->process_weight, task->pid, maxChild, pidOfChild, &subtreeMax, &pidOfSubtree);
		get_heaviest(subtreeMax, pidOfSubtree, *maxWeight, *pidOfMax, maxWeight, pidOfMax);
	}
	
	if(!hasChildren && root == current) {
		*pidOfMax = -ECHILD;
	}
}

void get_heaviest(int weight1, int pid1, int weight2, int pid2, int* maxWeight, int* pidOfMax) {
	if((weight1 > weight2) || (weight1 == weight2 && pid1 < pid2)) {
		*maxWeight = weight1;
		*pidOfMax = pid1;
		return;
	}
	
	*maxWeight = weight2;
	*pidOfMax = pid2;
} 
