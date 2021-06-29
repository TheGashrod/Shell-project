
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "array_utils.c"




/* The suite initialization function.
 * Returns zero on success, non-zero otherwise.
 */
int init_suite1(void)
{
   return 0;
}

/* The suite cleanup function.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1(void)
{
   return 0;
}


// test array_utils :


/** 
* test of split function
*/
void testSPLIT(void)
{
   char* input = "this is a test";
   char** stringArray  = NULL;
   stringArray = split(input, " ");
   CU_ASSERT_STRING_EQUAL(stringArray[0], "this");
   CU_ASSERT_STRING_EQUAL(stringArray[1], "is");
   CU_ASSERT_STRING_EQUAL(stringArray[2], "a");
   CU_ASSERT_STRING_EQUAL(stringArray[3], "test");
}

/** 
* test of remove_array_elements function
*/
void testREMOVE_ARRAY_ELEMENTS(void)
{
   char* input = "this is a test of remove_array_elements function";
   char** stringArray  = NULL;
   stringArray = split(input, " ");
   remove_array_elements(stringArray,1,3);

   CU_ASSERT_STRING_EQUAL(stringArray[0], "this");
   CU_ASSERT_STRING_EQUAL(stringArray[1], "of");
   CU_ASSERT_STRING_EQUAL(stringArray[2], "remove_array_elements");
   CU_ASSERT_STRING_EQUAL(stringArray[3], "function");
}


/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("array_utils_suite", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "test of split()", testSPLIT))     ||
       (NULL == CU_add_test(pSuite, "test of remove_array_elements()", testREMOVE_ARRAY_ELEMENTS)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}

