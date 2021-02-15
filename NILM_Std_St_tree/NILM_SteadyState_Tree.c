#include<conio.h>
#include<stdio.h>
#include <time.h>
#include <string.h>

#define	DV_NAME_LEN 15										//  Max Length of device names



//******************************************    Globle Variables Decleration   *******************************************************

int DEV_CNT							; 						// Available Trained device counter

// DEVICE STRUCTURE
typedef struct												// Structure for trained device list
{
	int			name[DV_NAME_LEN]	;						// Name of the device 
	int 		idx					;						// count of device numbers
	int 		pn					;						// PWR of the particular device
	int 		qn					;						// VAR of the particular device
}device_struct;

device_struct *device_list			;						// Declare a pointer for device list
device_struct null_device_list[1] ={0};						// NULL device structure		


// TREE NODE STRUCTURE
typedef struct NODE						 
{  
   device_struct *devlist_ptr		;						// Device Structure pointer				
   struct NODE *Left				;						// Left Node pointer
   struct NODE *Right				;						// Right Node pointer
   int height						;						// Node height
   int state						;						// State (ON/OFF) for device - Steady State
}NODE;

NODE *Root = NULL;											// Root NODE of Device TREE


// For TREE Parameters
int MAX_NODE_P, MIN_NODE_P, MAX_NODE_Q, MIN_NODE_Q	; 		// P&Q of Last & First NODE in TREE
int MAX_P, MIN_P, MAX_Q, MIN_Q						;		// MAX & MIN P&Q in TREE 
int SUM_LEFT_P, SUM_RIGHT_P, SUM_TREE_P				;		// P SUM of Left, Right and complete TREE NODEs
int SUM_LEFT_Q, SUM_RIGHT_Q, SUM_TREE_Q				;		// Q SUM of Left, Right and complete TREE NODEs
		
//******************************************    Globle Variables Decleration end  *******************************************************





//******************************************    Function Decleration    *****************************************************************

//					Functions for - TREE Insertion, Deletion,  Printing & Balancing

	NODE *Insert_node(NODE *, device_struct *);				// Insert a NODE in Device TREE
	void  Preorder(NODE *);									// Preorder Printing
	void  Inorder(NODE *);									// Inorder Printing
	int   Height(NODE *);									// Calculate Height of the NODE (Required for AVL TREE)

//  AVL TREE NODE balancing functions 
	NODE *RotateRight(NODE *);
	NODE *TotateLeft(NODE *);
	NODE *RR(NODE *);
	NODE *LL(NODE *);
	NODE *LR(NODE *);
	NODE *RL(NODE *);

	int Balance_Fact(NODE *);								// Calculate Balance Factor of the NODE (Required for AVL TREE)


//					Functions to - Calculate TREE PARAMETERS MAX, MIN & SUM of TREE NODEs 

  void Calculate_SUM(NODE *, int *, int *);
  void Last_Node_PQ(NODE *, int *, int *) ;
  void First_Node_PQ(NODE *, int *, int *);
  void Find_Max_PQ(NODE *Temp, int *, int);
  void Find_Min_PQ(NODE *Temp, int *, int);


//					Functions for - Search PWR & VAR in Device TREE & Combination of NODEs (STEADY STATE) 

  void Search_Combi(NODE *, int, int, int, int);			// Search Recursively in Device TREE for P&Q combination
  void State_Check(NODE *, int *, int *);					// Calculate SUM for NODEs with state = 1
  void State_Print(NODE *);									// Print Combination if found in device TREE


//					Functions for - Device List

  void Fill_Dev_List(device_struct *, device_struct *, int);// Fill New train device value in device structure
  void Print_List(device_struct *, int);					// Print Device Structure List
  void Comp_Sort_List(device_struct *, int);				// Sort Device List for P & Q both
  void Tree_Create(device_struct *, int);					// Create TREE
  void Destroy_Tree(NODE *);								// Destroy TREE
  void Calculate_Tree_para(NODE *);							// Calculate all TREE Parameters			
  void Print_Tree_para();									// Print all Calculated TREE Parameters
  void Clear_tree_para();									// Clear all Calculated TREE Parameters

//******************************************    Function Decleration end   **************************************************************




//*******************************************    Function Definition    ******************************************************************

/*
**********************************************************************************************************************************	
											Insert_node()
Description :  Inserting an element in Device TREE.
Argument(s) :  Root, device_list       Argument passed to 'Insert_node()' by 'Tree_Create()'.
Return(s)   :  Created Node.
**********************************************************************************************************************************
*/
NODE *Insert_node(NODE *Temp, device_struct *device_list)
{
    if(Temp == NULL)
    {
        Temp = (NODE *)malloc(sizeof(NODE));								// Create a new NODE
        Temp->devlist_ptr = device_list;									// Dereference Device List
        Temp->Left = NULL;
        Temp->Right = NULL;
    }
    else
        if(device_list->pn > Temp->devlist_ptr->pn || device_list->qn > Temp->devlist_ptr->qn)                			// Insert in Right subtree
        {
            Temp->Right = Insert_node(Temp->Right, device_list);
            if(Balance_Fact(Temp) == -2)
                if(device_list->pn > Temp->Right->devlist_ptr->pn || device_list->qn > Temp->devlist_ptr->qn)
                    Temp = RR(Temp);
                else
                    Temp = RL(Temp);
        }
        else
            if(device_list->pn < Temp->devlist_ptr->pn || device_list->qn < Temp->devlist_ptr->qn)						// Insert in Left subtree
            {
                Temp->Left = Insert_node(Temp->Left, device_list);
                if(Balance_Fact(Temp) == 2)
                    if(device_list->pn < Temp->Right->devlist_ptr->pn || device_list->qn < Temp->devlist_ptr->qn)
                        Temp = LL(Temp);
                    else
                        Temp = LR(Temp);
            }
            Temp->height = Height(Temp);
            return(Temp);
}

/*			
**********************************************************************************************************************************
									Height()
Description :  Calculate NODE Height.
Argument(s) :  NODE       Argument passed to 'Height()' by 'Insert_node()'.
Return(s)   :  int.
**********************************************************************************************************************************
*/
int Height(NODE *Temp)
{
    int lh,rh;
    if(Temp == NULL)
        return(0);
    if(Temp->Left == NULL)
        lh = 0;
    else
        lh = 1 + Temp->Left->height;
    if(Temp->Right == NULL)
        rh = 0;
    else
        rh = 1 + Temp->Right->height;
    if(lh > rh)
        return(lh);
    return(rh);
}


NODE *RotateRight(NODE *x)
{
    NODE *y;
    y = x->Left;
    x->Left = y->Right;

    y->Right = x;
    x->height = Height(x);
    y->height = Height(y);
    return(y);
}

NODE *RotateLeft(NODE *x)
{
    NODE *y;
    y = x->Right;
    x->Right = y->Left;

    y->Left = x;
    x->height = Height(x);
    y->height = Height(y);
    return(y);
}

NODE *RR(NODE *Temp)
{
    Temp = RotateLeft(Temp);
    return(Temp);
}

NODE *LL(NODE *Temp)
{
    Temp = RotateRight(Temp);
    return(Temp);
}

NODE *LR(NODE *Temp)
{
    Temp->Left = RotateLeft(Temp->Left);
    Temp = RotateRight(Temp);
    return(Temp);
}

NODE *RL(NODE *Temp)
{
    Temp->Right = RotateRight(Temp->Right);
    Temp = RotateLeft(Temp);
    return(Temp);
}

/*												
**********************************************************************************************************************************
											Balance_Fact()
Description :  Calculate Balance Factor.
Argument(s) :  NODE       Argument passed to 'Balance_Fact()' by 'Insert_node()'.
Return(s)   :  int.
**********************************************************************************************************************************
*/
int Balance_Fact(NODE *Temp)
{
    int lh,rh;
    if(Temp == NULL)
    return(0);
    if(Temp->Left == NULL)
        lh = 0;
    else
        lh = 1 + Temp->Left->height;
    if(Temp->Right == NULL)
        rh = 0;
    else
        rh = 1 + Temp->Right->height;
    return(lh-rh);
}

/*							
**********************************************************************************************************************************
									Preorder()
Description :  Preorder Printing.
Argument(s) :  Root       Argument passed to 'Preorder()' by 'main()'.
Return(s)   :  none.
**********************************************************************************************************************************
*/
void Preorder(NODE *Temp)
{
    if(Temp!=NULL)
    {
        printf(" idx=%d (P=%d, Q=%d)\n",Temp->devlist_ptr->idx, Temp->devlist_ptr->pn, Temp->devlist_ptr->qn);
        Preorder(Temp->Left);
        Preorder(Temp->Right);
    }
}

/*			
**********************************************************************************************************************************
									Inorder()
Description :  Inorder Printing.
Argument(s) :  Root       Argument passed to 'Inorder()' by 'main()'.
Return(s)   :  none.
**********************************************************************************************************************************
*/
void Inorder(NODE *Temp)
{
    if(Temp!=NULL)
    {
        Inorder(Temp->Left);
		printf(" idx=%d (P=%d, Q=%d)\n",Temp->devlist_ptr->idx, Temp->devlist_ptr->pn, Temp->devlist_ptr->qn);
        Inorder(Temp->Right);
    }
}

/*										
**********************************************************************************************************************************
									Calculate_SUM()
Description :  Calculate SUM of NODES - Inorder.
Argument(s) :  Root, variales address to store calculated values     Argument passed to 'Calculate_SUM()' by 'Calculate_Tree_para()'.
Return(s)   :  none.
**********************************************************************************************************************************
*/
void Calculate_SUM(NODE *Temp, int *sum_p, int *sum_q)
{
    if(Temp != NULL)
    {
      Calculate_SUM(Temp->Left, sum_p, sum_q);
      *sum_p += Temp->devlist_ptr->pn;
      *sum_q += Temp->devlist_ptr->qn;
      Calculate_SUM(Temp->Right, sum_p, sum_q);
    }
}

/*			
**********************************************************************************************************************************
									Last_Node_PQ()
Description :  Find MAX NODE P&Q	(Rightmost NODE).
Argument(s) :  Root, variales address to store calculated values     Argument passed to 'Last_Node_PQ()' by 'Calculate_Tree_para()'.
Return(s)   :  none.
**********************************************************************************************************************************
*/
void Last_Node_PQ(NODE *Temp, int *p, int *q)
{
    if(Temp != NULL)
    {
       *p = Temp->devlist_ptr->pn;
       *q = Temp->devlist_ptr->qn;
       Last_Node_PQ(Temp->Right, p, q); 
    }
}

/*			
**********************************************************************************************************************************
									First_Node_PQ()
Description :  Find MIN NODE P&Q    (Leftmost NODE).
Argument(s) :  Root, variales address to store calculated values     Argument passed to 'First_Node_PQ()' by 'Calculate_Tree_para()'.
Return(s)   :  none.
**********************************************************************************************************************************
*/
void First_Node_PQ(NODE *Temp, int *p, int *q)
{
    if(Temp != NULL)
    {
       *p = Temp->devlist_ptr->pn;
       *q = Temp->devlist_ptr->qn;
       First_Node_PQ(Temp->Left, p, q); 
    }
}

/*			
**********************************************************************************************************************************
								Find_Max_PQ()
Description :   Maximum P & Q in TREE- Reverse Inorder (for less number of copy).
Argument(s) :   Root, variale address to store calculated values     Argument passed to 'Find_Max_PQ()' by 'Calculate_Tree_para()'.
				sel=1(P) sel=0(Q)
Return(s)   :   none.
**********************************************************************************************************************************
*/
void Find_Max_PQ(NODE *Temp, int *x, int sel)							
{
    if(Temp != NULL)
    {
	Find_Max_PQ(Temp->Right, x, sel);
	if(sel)	
	{
		if(*x < Temp->devlist_ptr->pn)
		*x = Temp->devlist_ptr->pn;
	}
	else
	{
		if(*x < Temp->devlist_ptr->qn)
		*x = Temp->devlist_ptr->qn;
	}
	Find_Max_PQ(Temp->Left, x, sel);
    }
}

/*				
**********************************************************************************************************************************
								Find_Min_PQ()
Description :   Minimum P & Q in TREE- Inorder (for less number of copy).
Argument(s) :   Root, variale address to store calculated values     Argument passed to 'Find_Min_PQ()' by 'Calculate_Tree_para()'.
				sel=1(P) sel=0(Q)
Return(s)   :   none.
**********************************************************************************************************************************
*/
void Find_Min_PQ(NODE *Temp, int *x, int sel)							
{
    if(Temp != NULL)
    {
	Find_Min_PQ(Temp->Left, x, sel);
	if(sel)	
	{
		if(*x > Temp->devlist_ptr->pn)
		*x = Temp->devlist_ptr->pn;
	}
	else
	{
		if(*x > Temp->devlist_ptr->qn)
		*x = Temp->devlist_ptr->qn;
	}
	Find_Min_PQ(Temp->Right, x, sel);
    }
}

/*			
**********************************************************************************************************************************
									Search_Combi()
Description :   Searching for the Combination in Device TREE equal to find_p & find_q in Reverse Inorder.
Argument(s) :   Root, dev_idx, P, P, Q     Argument passed to 'Search_Combi()' by 'main()'.
Return(s)   :   none.
**********************************************************************************************************************************
*/
void Search_Combi(NODE *Temp, int curr_id, int search, int find_p, int find_q)
{
	if(Temp != NULL)
	{
		int sum_p = 0, sum_q = 0;
		Search_Combi(Temp->Right, curr_id, search, find_p, find_q);
		if(Temp->devlist_ptr->idx < curr_id)
		{
			if(Temp->devlist_ptr->pn == search && Temp->devlist_ptr->idx != curr_id)
			{
		   	  Temp->state = 1;
			  sum_p = sum_q = 0;
			  State_Check(Root, &sum_p, &sum_q);											// Calculate SUM of all NODEs with state 1
			  if(sum_p == find_p && sum_q == find_q)
			  {
				sum_p = sum_q = 0;
			  	State_Print(Root);															// Print the Combination 
				printf("\n");
			  }
			  Temp->state = 0;
			}

			else if(Temp->devlist_ptr->pn < search && Temp->devlist_ptr->idx != 0)
			{	
			  Temp->state = 1;
			  Search_Combi(Root, Temp->devlist_ptr->idx, (search - Temp->devlist_ptr->pn), find_p, find_q);
			  Temp->state = 0;
			}
		}
		Search_Combi(Temp->Left, curr_id, search, find_p, find_q);
	}
}

/*			
**********************************************************************************************************************************
									State_Check()
Description :   Calculate SUM of all NODEs with state 1 - Reverse Inorder.
Argument(s) :   Root, sum_p, sum_q     Argument passed to 'State_Check()' by 'Search_Combi()'.
Return(s)   :   none.
**********************************************************************************************************************************
*/
void State_Check(NODE *Temp, int *sum_p, int *sum_q)
{
	if(Temp != NULL)
	{
	State_Check(Temp->Right, sum_p, sum_q);
		if(Temp->state == 1)
		{	
			*sum_p += Temp->devlist_ptr->pn;
			*sum_q += Temp->devlist_ptr->qn;
		}
	State_Check(Temp->Left, sum_p, sum_q);	
	}
}

/*			
**********************************************************************************************************************************
									State_Print()
Description :   Print the Combination - Reverse Inorder.
Argument(s) :   Root    Argument passed to 'State_Print()' by 'Search_Combi()'.
Return(s)   :   none.
**********************************************************************************************************************************
*/
void State_Print(NODE *Temp)
{
  	if(Temp != NULL)
	{
	State_Print(Temp->Right);
		if(Temp->state == 1)
		{
			printf("  %d(%d)", Temp->devlist_ptr->pn, Temp->devlist_ptr->qn);
		}
	State_Print(Temp->Left);
	}
}

/*			
**********************************************************************************************************************************
									Fill_Dev_List()
Description :   Copy new trained valves in Device List.
Argument(s) :   device_list, train_cpy, dev_cnt    Argument passed to 'Fill_Dev_List()' by 'main()'.
Return(s)   :   none.
**********************************************************************************************************************************
*/
void Fill_Dev_List(device_struct *device_list, device_struct *train_cpy, int dev_cnt)
{
	device_list[dev_cnt] = train_cpy[0];
}

/*			
**********************************************************************************************************************************
									Print_List()
Description :   Print Device List Parameters.
Argument(s) :   device_list, dev_cnt    Argument passed to 'Print_List()' by 'main()'.
Return(s)   :   none.
**********************************************************************************************************************************
*/
void Print_List(device_struct *device_list, int dev_cnt)
{
	int i;
	printf("\nIdx		Device name		P_act		P_react\n");

	for(i = 0; i < dev_cnt; i++)
		printf(" %d              %s                    %d                    %d\n", device_list[i].idx, device_list[i].name, device_list[i].pn, device_list[i].qn);
}

/*		
**********************************************************************************************************************************
										Comp_Sort_List()
Description :   Sort Device List for P & Q.
Argument(s) :   device_list, dev_cnt    Argument passed to 'Comp_Sort_List()' by 'main()'.
Return(s)   :   none.
**********************************************************************************************************************************
*/
void Comp_Sort_List(device_struct *device_list, int dev_cnt)
{
	int i = 0;
    int idx;
	device_struct temp_stuct;

	do
	{
		for(idx=i+1; idx <= dev_cnt; idx++)
		{
			if( device_list[i].pn > device_list[idx].pn)					// for P
			{
				temp_stuct		= device_list[i]	;
				device_list[i]	        = device_list[idx]	;				// Swap the device structures
				device_list[idx]        = temp_stuct		;

				device_list[i].idx	=	i		;							// Swap the device idx based on the sorted index
				device_list[idx].idx	=	idx		;

			}
			else if(device_list[i].pn == device_list[idx].pn)				// if P is same go for Q
			{
				if( device_list[i].qn > device_list[idx].qn)
				{
					temp_stuct		= device_list[i]	;
					device_list[i]	        = device_list[idx]	;			// Swap the device structures
					device_list[idx]        = temp_stuct		;

					device_list[i].idx	=	i		;						// Swap the device idx based on the sorted index
					device_list[idx].idx	=	idx		;
				}
			}
		}
		i++;
	}while( i < dev_cnt );

}

/*			
**********************************************************************************************************************************
									Tree_Create()
Description :   Create TREE.
Argument(s) :   device_list, dev_cnt    Argument passed to 'Tree_Create()' by 'main()'.
Return(s)   :   none.
**********************************************************************************************************************************
*/
void Tree_Create(device_struct *device_list, int dev_cnt)
{
	int i; 
	for(i = 0; i < dev_cnt; i++)
	{
		Root = Insert_node(Root, &device_list[i]);							// Insert NODES in Device TREE 
	}
}
 
/*		
**********************************************************************************************************************************
										Destroy_Tree()
Description :   Destroy tree - postorder traversal.
Argument(s) :   NODE    Argument passed to 'Destroy_Tree()' by 'main()'.
Return(s)   :   none.
**********************************************************************************************************************************
*/
void Destroy_Tree(NODE *Temp)						
{
  if(Temp == NULL)
    return;

  Destroy_Tree(Temp->Left);
  Destroy_Tree(Temp->Right);

  free(Temp);
  Temp = NULL;

  return;
}

/*		
**********************************************************************************************************************************
										Calculate_Tree_para()
Description :   Calculate TREE Parameters.
Argument(s) :   Root    Argument passed to 'Calculate_Tree_para()' by 'main()'.
Return(s)   :   none.
**********************************************************************************************************************************
*/
void Calculate_Tree_para(NODE *Temp)
{
    // Initialize MAX & MIN P&Q to Root P&Q
    MAX_P = MIN_P = Root->devlist_ptr->pn; 
    MAX_Q = MIN_Q = Root->devlist_ptr->qn;
  
    // Find Max NODE - The rightmost node in tree
    Last_Node_PQ(Root, &MAX_NODE_P, &MAX_NODE_Q);
    
    // Find Min NODE - The leftmost node in tree
    First_Node_PQ(Root, &MIN_NODE_P, &MIN_NODE_Q);

    // Maximum P in TREE (by value)
    Find_Max_PQ(Root, &MAX_P, 1);
    // Maximum Q in TREE
    Find_Max_PQ(Root, &MAX_Q, 0);

    // Minimum P in TREE
    Find_Min_PQ(Root, &MIN_P, 1);
    // Minimum Q in TREE
    Find_Min_PQ(Root, &MIN_Q, 0);

    // Calculate Left Nodes SUM
    if(Root->Left != NULL)
	Calculate_SUM(Root->Left, &SUM_LEFT_P, &SUM_LEFT_Q);

    // Calculate Right Nodes SUM
    if(Root->Right != NULL)
	Calculate_SUM(Root->Right, &SUM_RIGHT_P, &SUM_RIGHT_Q);

    // Calculate tree all Nodes SUM
    if(Root != NULL)
	Calculate_SUM(Root, &SUM_TREE_P, &SUM_TREE_Q);
}

/*			
**********************************************************************************************************************************
									Print_Tree_para()
Description :   Print TREE Parameters.
Argument(s) :   none    
Return(s)   :   none.
**********************************************************************************************************************************
*/
void Print_Tree_para()
{
	printf("\nInorder sequence :\n");											// Print TREE Inorder Sequence
        Inorder(Root);
        printf("\n");

	printf("\nMAX_NODE_P	          = %d,	      MAX_NODE_Q = %d\n", MAX_NODE_P, MAX_NODE_Q);
	printf("MIN_NODE_P		  = %d,	      MIN_NODE_Q = %d\n\n", MIN_NODE_P, MIN_NODE_Q);

	printf("MAX_P = %d,		MAX_Q = %d\n", MAX_P, MAX_Q);
	printf("MIN_P = %d,		MIN_Q = %d\n\n", MIN_P, MIN_Q);

	printf("SUM_LEFT_P	= %d, 	SUM_LEFT_Q	= %d\n", SUM_LEFT_P, SUM_LEFT_Q);
	printf("SUM_RIGHT_P     = %d, 	SUM_RIGHT_Q     = %d\n", SUM_RIGHT_P, SUM_RIGHT_Q);
	printf("SUM_TREE_P	= %d, 	SUM_TREE_P	= %d\n\n", SUM_TREE_P, SUM_TREE_Q);
}

/*			
**********************************************************************************************************************************
									Clear_tree_para()
Description :   Clear TREE Parameters.
Argument(s) :   none    
Return(s)   :   none.
**********************************************************************************************************************************
*/ 
void Clear_tree_para()
{
	MAX_NODE_P = MIN_NODE_P = MAX_NODE_Q = MIN_NODE_Q = 0;
	MAX_P = MIN_P = MAX_Q = MIN_Q = 0;
 	SUM_LEFT_P = SUM_RIGHT_P = SUM_TREE_P = 0;
	SUM_LEFT_Q = SUM_RIGHT_Q = SUM_TREE_Q = 0;	
}

//*******************************************    Function Definition end   ***************************************************************




//*******************************************    Main Function    ************************************************************************
int main_tree()
{
    int i, x = 0, id = 0, op = 0, find_p = 0, find_q = 0, train_p = 0, train_q = 0, del_id = 0;
    NODE *temp = NULL;
    //device_struct *temp_list;				// Temporary device list for realloc
    device_struct train_cpy[1];				// Store new trained device parameters

    do
        {
			// USER GUIDE	
            printf("\n1)TRAINING: ");
            printf("\n2)DELETE: ");
			printf("\n3)SEARCH COMBI: ");
            printf("\n4)PRINT LIST: ");
			printf("\n5)TREE PARAMETERS: ");
			printf("\n6)QUIT: ");

            printf("\nEnter Your Choice: ");
            scanf("%d", &op);
            
			switch(op)
            {
                case 1: 
						// TRAINING
						printf("\nEnter device name.:");														// device name
						scanf("%s", train_cpy->name);

						printf("\nEnter P.:"); 																	// Trained P
                       	scanf("%d", &train_cpy[0].pn);

		       			printf("\nEnter Q.:");
                       	scanf("%d", &train_cpy[0].qn); 															// Trained Q

						train_cpy->idx = DEV_CNT;																// Device indexing

						device_struct *temp_list = realloc(device_list, (DEV_CNT+1) * sizeof(device_struct));	// Allocate memory dynamically for new device

						if ( temp_list != NULL ) 																// Realloc is successful
						{
							device_list = temp_list;
							Fill_Dev_List(device_list, train_cpy, DEV_CNT);										// Fill device_list with new trained list
				
							if(DEV_CNT != 0)
							{
								Comp_Sort_List(device_list, DEV_CNT);											// Compare Sort device list in Ascending Order
							}
				
							DEV_CNT++;																			// After training increase DEV_CNT
				
							if(Root != NULL)
							{
								if(Root->Left != NULL)
									Destroy_Tree(Root->Left);
								if(Root->Right != NULL)
									Destroy_Tree(Root->Right);
								Root = NULL;
								Clear_tree_para();																// Clear TREE parameters
							}
				
							Tree_Create(device_list, DEV_CNT);													// Create TREE for Sorted Device List
							Calculate_Tree_para(Root);															// Calculate TREE parameters

						}
						else 																					// There was an error to allocate memory
						{
							printf("Error allocating memory!\n");
							return 1;
						}
                       	break;

		
				case 2: 
						// DELETE
						if(DEV_CNT == 0)																		// Empty List
						printf("\nDevice list is empty\n");

						else if(DEV_CNT == 1)																	// Only 1 device is Trained
						{
							free(device_list[0]);
							Root = NULL;
							Clear_tree_para();																	// Clear Tree parameters
							DEV_CNT--;
						}

						else
						{	
							Print_List(device_list, DEV_CNT);													// For index selection list out all trained devices

							printf("\nEnter Device idx:"); 		
                       		scanf("%d", &del_id);																// User selected device Id for deleting
							
							if(del_id >= DEV_CNT)
								printf("\nDevice idx not exist");
								
							else
							{
								if(del_id != DEV_CNT-1)																// Last device
								{
									device_list[DEV_CNT-1].idx = device_list[del_id].idx;
									device_list[del_id] = device_list[DEV_CNT-1];
								}
				
								device_struct *temp_list = realloc(device_list, (DEV_CNT-1) * sizeof(device_struct));
								if ( temp_list != NULL ) 															// Realloc is successful
								{
									device_list = temp_list;
									DEV_CNT--;
									if(DEV_CNT != 0)
									{
										Comp_Sort_List(device_list, DEV_CNT);										// Compare Sort device list in Ascending order	
									}
									if(Root !=NULL)
									{
										if(Root->Left != NULL)
											Destroy_Tree(Root->Left);
										if(Root->Right != NULL)
											Destroy_Tree(Root->Right);
										Root = NULL;
										Clear_tree_para();															// Clear TREE parameters
									}

									Tree_Create(device_list, DEV_CNT);												// Create TREE for sorted device list
									Calculate_Tree_para(Root);														// Calculate TREE parameters
								}
								else 																				// There was an error to allocate memory
								{
									printf("Error allocating memory!\n");
									return 1;
								}
							}
						}	
				break;
		
				case 3: 
						// SEARCH COMBI 
						if(DEV_CNT ==0)
							printf("\nDevice list is empty\n");
						else
						{
							printf("\nEnter P.:");																// P
                       		scanf("%d", &find_p);

		       				printf("\nEnter Q.:");
                       		scanf("%d", &find_q);																// Q

		       				// clock_t tic = clock();
                       		Search_Combi(Root, DEV_CNT, find_p, find_p, find_q);								// Function to search all possible combination in Device TREE	
		       				// clock_t toc = clock();
		       				// printf("Elapsed: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
						}
				break;

				case 4: 
						// PRINT LIST 
						Print_List(device_list, DEV_CNT); 
				break;

		
				case 5: 
						// TREE PARAMETERS
						if(Root != NULL)
							Print_Tree_para();
						else
							printf("\n Device list is empty\n");
						break;
            }

	}while(op!=6);

return 0;

}

//*******************************************    Main Function end   *******************************************************************




