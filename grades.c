#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "grades.h"
#include "linked-list.h"



struct grades{
	struct list *students;
};

/* element in linked list students*/
typedef struct elem_student_t{
	const char *name;
	int id;
	struct list *courses;
} elem_student_t;

/* element in linked list courses*/
typedef struct elem_course_t{
	const char *course_name;
	int grade;
} elem_course_t;

static void print_grades(elem_student_t *student);
static struct node* student_list_search(struct list *stud_list, int id);

int clone_elem_student_t(void *element, void **output){
	if(!element){
		return 1;
	}
	elem_student_t *student;
	elem_student_t *out;
	student=(elem_student_t*)element;
	out=(elem_student_t*)malloc(sizeof(*out));

	if(!out){
		free(out);
		return 1;
	}
	int str_len=strlen(student->name);
	char *name_string= malloc((sizeof(char))*(str_len+1));
	strcpy(name_string,student->name);
	out->name=name_string;
	out->id=student->id;
	out->courses=student->courses;

	*output = out;
	return 0;
}

int clone_elem_course_t(void *element, void **output){
	elem_course_t *course;
	elem_course_t *out;
	course=(elem_course_t*)element;
	out=(elem_course_t*)malloc(sizeof(*out));

	if(!out){
		free(out);
		return 1;
	}
	int str_len=strlen(course->course_name);
	char *name_string= malloc((sizeof(char))*(str_len+1));
	strcpy(name_string,course->course_name);
	out->course_name=name_string;
	out->grade=course->grade;
	*output = out;
	return 0;
}

void destroy_elem_student_t(void *element){
	if(!element){
		return;
	}
	elem_student_t *student;
	student=(elem_student_t*)element;
	list_destroy(student->courses);
	free((char*)student->name);
	free(student);
}

void destroy_elem_course_t(void *element){
	if(!element){
		return;
	}
	elem_course_t *course;
	course=(elem_course_t*)element;
	free((char*)course->course_name);
	free(course);
}

struct grades* grades_init(){
	struct grades *grades;
	struct list *students;
	grades=(struct grades*)malloc(sizeof(*grades));
	students=list_init(clone_elem_student_t, destroy_elem_student_t);
	if (!grades || !students){
		free(grades);
		free(students);
		return NULL;
	}
	grades->students=students;
	return grades;
}

void grades_destroy(struct grades *grades){
	if(!grades){
		return;
	}
	list_destroy(grades->students);
	free(grades);

}


int grades_add_student(struct grades *grades, const char *name, int id){
	if(!grades){
		return 1;
	}
	struct node *node;
	node = student_list_search(grades->students, id);
	if(node){
		return 1;
	}
	elem_student_t *student;
	student=(elem_student_t*)malloc(sizeof(elem_student_t));
	if(!student){
		return 1;
	}
	student->id=id;
	student->name=name;
	student->courses=list_init(clone_elem_course_t, destroy_elem_course_t);
	if (!student->courses){
		free(student);
		return 1;
	}
	int r_val = (list_push_back(grades->students, student));
	free(student);
	return r_val;
}

int grades_add_grade(struct grades *grades,
                     const char *name,
                     int id,
                     int grade){
	if(!grades || grade>100 || grade<0){
		return 1;
	}
	struct node *node_student;
	elem_student_t *student;
	node_student = student_list_search(grades->students, id);
	if (!node_student){
		return 1;
	}

	student = list_get(node_student);
	struct node *node_course;
	elem_course_t *course;
	node_course=list_begin(student->courses);
	while(node_course){
		course=list_get(node_course);
		if(!strcmp(name, course->course_name)){
			return 1;
		}
		node_course=list_next(node_course);
	}
	elem_course_t *new_course;
	new_course=(elem_course_t*)malloc(sizeof(elem_course_t));
	if(!new_course){
		return 1;
	}
	new_course->grade = grade;
	new_course->course_name=name;
	int r_val = list_push_back(student->courses, new_course);
	free(new_course);
	return r_val;
}


float grades_calc_avg(struct grades *grades, int id, char **out){
	if(!grades){
		out=NULL;
		return -1;
	}
	struct node *node = student_list_search(grades->students,id);
	if (!node){
		out=NULL;
		return -1;
	}
	elem_student_t *element_student= list_get(node);
	int str_len=strlen(element_student->name);
	char *name_string= malloc((sizeof(char))*(str_len+1));
	strcpy(name_string,element_student->name);
	*out= name_string;
	struct list *list_course = element_student->courses;
	int num_courses = list_size(list_course);
	if(num_courses==0){
		return 0;
	}
	elem_course_t *course;
	node=list_begin(list_course);
	int sum=0;
	while(node){
		course = list_get(node);
		int curr_course_grade = course->grade;
		sum+= curr_course_grade;
		node= list_next(node);
	}
	return ((float)sum/(float)num_courses);

}

int grades_print_student(struct grades *grades, int id){
	if(!grades){
		return 1;
	}
	struct node *node_student;
	elem_student_t *student;

	node_student = student_list_search(grades->students, id);
	if (!node_student){
		return 1;
	}
	student = list_get(node_student);
	print_grades(student);
	return 0;
}


int grades_print_all(struct grades *grades){
	if(!grades){
		return 1;
	}
	struct node *node_student;
	elem_student_t *student;
	node_student = list_begin(grades->students);
	while(node_student){
		student = list_get(node_student);
		print_grades(student);
		node_student = list_next(node_student);
	}
	return 0;
}

/* param student a pointer to a student node
 * prints all the grades of the student
 * */
static void print_grades(elem_student_t *student){
	struct node *node_course;
	elem_course_t *course;
	node_course=list_begin(student->courses);
	printf("%s %d:", student->name, student->id);
	while(node_course){
		course=list_get(node_course);
		printf(" %s %d", course->course_name, course->grade);
		node_course=list_next(node_course);
		if(node_course){
			printf(",");
		}
	}
	printf("\n");
}

/* param stud_list a pointer to student list
 * param id is the student id
 * returns NULL if list is empty or there isn't student with the id in list
 * else returns pointer to the student node*/
static struct node* student_list_search(struct list *stud_list, int id){
	struct node *node;
	elem_student_t *student;
	node=list_begin(stud_list);
	while(node){
		student=list_get(node);
		if(id == student->id){
			return node;
		}
		node=list_next(node);
	}
	return NULL;
}
