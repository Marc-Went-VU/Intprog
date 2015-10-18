#include <stdio.h>
#include <stdlib.h>

#include "rpcfunc.h"

// struct papers
// {
// 	struct papers *next;
// 	struct papers *prev;
// 	int id;
//
// 	char* author;
// 	char* title;
// 	char* paper;
// };
struct paper_list_out* head;
struct paper_list_out* tail;
typedef enum { false, true } bool;
//  [0,........,x]
// [tail,.....,head]


void freePreviousListOut(struct paper_list_out* out)
{
	struct paper_list_out* tmp;
	while(out != NULL)
	{
		free(out->paper_info);
		tmp = out->next;
		free(out);
		out = tmp;
	}
}

void remove_paper(struct paper_list_out* curr)
{
	if(curr->next != NULL && curr->prev != NULL)
	{
		curr->prev->next = curr->next;
		curr->next->prev = curr->prev;
	}
	else if(curr->next != NULL && curr->prev == NULL)
	{
		curr->next->prev = NULL;
	}
	else if(curr->next == NULL && curr->prev != NULL){
		curr->prev->next = NULL;
	}
	freePreviousListOut(curr);
}
bool isPaper(struct paper_list_out* curr, int id)
{
	return curr->id == id;
}

struct paper_list_out* closer(int id, struct paper_list_out* a, struct paper_list_out* b)
{
	int toA = a == NULL ? INT_MAX : abs(a->id - id);
	int toB = b == NULL ? INT_MAX : abs(b->id - id);
	return toA < toB ? a : b;
}

bool hasPapers(){
	return head != NULL || tail != NULL;
}

int_out *remove_paper_1_svc(int_in *in, struct svc_req *req)
{
	static int_out out = 1;
	int id = (int) *in;
	struct paper_list_out* curr;
	struct paper_list_out* tmp;

	bool forward;

	if(!hasPapers())
	{
		out = -1;
		return &out;
	}

	curr = closer(id, head, tail); //Not necessarily best option:
	// [0,1,2,30,100] <= 30 is closer to 100 in this case. But worst O is O(n)

	forward = curr == head ? false : true;

	while(!isPaper(curr, id)){
		curr = forward ? curr->next : curr->prev;
		if(curr == NULL)
		{
			return &out;
		}
	}

	if(curr == head)
	{
		if(head->next != NULL) //SHOULDN'T Happen
		{
			tmp = head->next;
			tmp->prev = head->prev;
		}
		else if(head->prev != NULL)
		{
			tmp = head->prev;
			tmp->next = NULL;
		}
		remove_paper(head);
		head = tmp;
	}
	else if(curr == tail)
	{
		if(tail->prev != NULL) //SHOULDN'T Happen
		{
			tmp = tail->prev;
			tmp->next = tail->next;
		}
		else if(tail->next != NULL)
		{
			tmp = tail->next;
			tmp->prev = NULL;
		}
		remove_paper(tail);
		tail = tmp;
	}
	else
	{
		remove_paper(curr);
	}

	return &out;
}

struct paper_information *fetch_paper_1_svc(int_in *in, struct svc_req *req)
{
	static paper_information* out;

	int id = (int) *in;
	struct paper_list_out* curr;
	bool forward;
	u_int tmpValue = 0;
	paper_out *paperInfo;

	if(out != NULL)
	{
		free(out);
	}

	out = (paper_information*) malloc(sizeof(struct paper_information));
	if(out == NULL)
	{
		perror("Error allocating memory");
		exit(1);
	}
	out->author = (char *) malloc(sizeof(char));
	out->title 	=  (char *) malloc(sizeof(char));
	if(!hasPapers())
	{
		out->paper.paper_val = malloc(sizeof(char));
		out->paper.paper_len = 0;
		return out;
	}
	curr = closer(id, head, tail); //Not necessarily best option:
	// [0,1,2,30,100] <= 30 is closer to 100 in this case. But worst O is O(n)
	forward = curr == head ? false : true;

	while(!isPaper(curr, id)){
		curr = forward ? curr->next : curr->prev;
		if(curr == NULL)
		{
			out->paper.paper_val = malloc(sizeof(char));
			out->paper.paper_len = 0;
			return out;
		}
	}
	paperInfo = malloc(sizeof(paper_out));
	paperInfo->paper_out_len = 0;

	paperInfo->paper_out_val = malloc(curr->paper_info->paper.paper_len * sizeof(char));
	memcpy(paperInfo->paper_out_val, &(curr->paper_info->paper.paper_val), curr->paper_info->paper.paper_len);
	paperInfo->paper_out_len = curr->paper_info->paper.paper_len;
	//
	// paper = curr->paper_info->paper;
	// printf("Found the paper: %u\n", paper.paper_len);
	// out->paper.paper_len = 0;
	// out->paper.paper_val = malloc(paper.paper_len * sizeof(char));
	// memcpy(&(out->paper.paper_val),
	// 	&(curr->paper_info->paper.paper_val),
	// 	paper.paper_len
	// );
	//
	// out->paper.paper_len = paper.paper_len;
	printf("It sould be copied: %u", paperInfo->paper_out_len);
	return out;
}


paper_information *info_paper_1_svc(int_in *in, struct svc_req *req)
{
	static paper_information* out;

	int id = (int) *in;
	struct paper_list_out* curr;
	bool forward;

	if(out != NULL)
	{
		free(out);
	}
	out = (paper_information*) malloc(sizeof(struct paper_information));
	if(out == NULL)
	{
		perror("Error allocating memory");
		exit(1);
	}

	if(!hasPapers())
	{
		out->author = (char *) malloc(sizeof(char));
		out->title 	=  (char *) malloc(sizeof(char));
		out->paper.paper_val =  malloc(sizeof(char));
		return out;
	}
	curr = closer(id, head, tail); //Not necessarily best option:
	// [0,1,2,30,100] <= 30 is closer to 100 in this case. But worst O is O(n)
	forward = curr == head ? false : true;

	while(!isPaper(curr, id)){
		curr = forward ? curr->next : curr->prev;
		if(curr == NULL)
		{
			return out;
		}
	}

	out->author = strdup(curr->paper_info->author);
	out->title 	= strdup(curr->paper_info->title);

	return out;
}

paper_list_out *list_paper_1_svc(list_in *in, struct svc_req *req)
{
	static paper_list_out* out;
	struct paper_list_out* curr_out;
	struct paper_list_out* curr;

	curr = tail;

	freePreviousListOut(out);

	out = (struct paper_list_out*) malloc(sizeof(struct paper_list_out));
	if(out == NULL)
	{
		perror("Error allocating memory");
		exit(1);
	}

	curr_out = out;

	while(curr != NULL){

		curr_out->id = curr->id;

		curr_out->paper_info	= (struct paper_information*) malloc(sizeof(struct paper_information));
		curr_out->next 			= (struct paper_list_out*) malloc(sizeof(struct paper_list_out));
		if(curr_out->next == NULL || curr_out->paper_info == NULL)
		{
			perror("Error allocating memory");
			exit(1);
		}

		curr_out->paper_info->author 	= strdup(curr->paper_info->author);
		curr_out->paper_info->title 	= strdup(curr->paper_info->title);
		curr_out->paper_info->paper.paper_len = 0;
		curr_out->paper_info->paper.paper_val = malloc(sizeof(char));

		if(curr->next != NULL){
			curr_out->next->prev 	= curr_out;
			curr_out 				= curr_out->next;
			curr 					= curr->next;
		}
		else{
			curr 					= NULL;
		}
	}

	return out;
}

int_out *add_paper_1_svc(paper_information *in, struct svc_req *req)
{
	struct paper_list_out* newHead;

	newHead 						= (struct paper_list_out*) malloc(sizeof(struct paper_list_out));
	newHead->id 					= head == NULL ? 0 : head->id + 1;
	newHead->paper_info 			= (struct paper_information*) malloc(sizeof(struct paper_information));
	(newHead->paper_info)->author 	= strdup(in->author);
	(newHead->paper_info)->title	= strdup(in->title);
	(newHead->paper_info)->paper	= in->paper;

	if(head != NULL){
		newHead->prev = head;
		head->next = newHead;
	}

	head = newHead;
	if(tail == NULL)
	{
		tail = head;
	}
	return (int_out*) &newHead->id;
}
