/* *********************************************************************
 * This Original Work is copyright of 51 Degrees Mobile Experts Limited.
 * Copyright 2019 51 Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY.
 *
 * This Original Work is licensed under the European Union Public Licence (EUPL) 
 * v.1.2 and is subject to its terms as set out below.
 *
 * If a copy of the EUPL was not distributed with this file, You can obtain
 * one at https://opensource.org/licenses/EUPL-1.2.
 *
 * The 'Compatible Licences' set out in the Appendix to the EUPL (as may be
 * amended by the European Commission) shall be deemed incompatible for
 * the purposes of the Work and the provisions of the compatibility
 * clause in Article 5 of the EUPL shall not apply.
 * 
 * If using the Work as, or as part of, a network application, by 
 * including the attribution notice(s) required under Article 5 of the EUPL
 * in the end user terms of the application under an appropriate heading, 
 * such notice(s) shall fulfill the requirements of that article.
 * ********************************************************************* */
 
#include "pch.h"
#include "Base.hpp"
#include "StringCollection.hpp"
#include "../properties.h"

// Property names
static const char* testValues[] = {
	"Red",
	"Yellow",
	"Green",
	"Blue",
	"Brown",
	"Black",
	"White"
};

/**
 * Properties test class used to test the functionality of properties.c.
 */
class Properties : public Base {
protected:
	StringCollection *strings;
	int count;
	fiftyoneDegreesPropertiesAvailable *properties = nullptr;

	/**
	 * Calls the base setup method to enable memory leak checking and memory
	 * allocation checking. Also constructs a string collection to be used by
	 * the test methods.
	 */
	void SetUp() {
		Base::SetUp();
		count = sizeof(testValues) / sizeof(const char*);
		strings = new StringCollection(testValues, count);
	}

	/**
	 * Releases the properties structure if one was created, and the
	 * StringCollection instance. Then calls the base teardown method to check
	 * for memory leaks and compare expected and actual memory allocations.
	 */
	void TearDown() {
		if (properties != nullptr) {
			fiftyoneDegreesPropertiesFree(properties);
		}
		delete strings;
		Base::TearDown();
	}

	/**
	 * Create a properties structure from the specified required properties
	 * using the method in properties.c. The expected memory allocation is
	 * calculated, and the actual memory allocation is tracked. The structure
	 * is freed automatically after each test, at which point the expected and
	 * actual memory allocation is checked for equality.
	 * @param required the properties used to create the properties structure
	 */
	void CreateProperties(fiftyoneDegreesPropertiesRequired *required) {
		properties = fiftyoneDegreesPropertiesCreate(
			required,
			strings->getState(),
			getStringValue);
	}
};

/**
 * Check that all the properties are present as expected.
 */
TEST_F(Properties, AllProperties) {
	CreateProperties(NULL);
	for (int i = 0; i < count; i++) {
		int reqIndex =
			fiftyoneDegreesPropertiesGetRequiredPropertyIndexFromName(
				this->properties,
				testValues[i]);
		const char *name = FIFTYONE_DEGREES_STRING(
			fiftyoneDegreesPropertiesGetNameFromRequiredIndex(
				this->properties,
				reqIndex));
		EXPECT_STREQ(testValues[i], name);
	}
}

/**
 * Check that passing a string list of required properties works as expected.
 * Yellow is in the list of properties and required properties so should 
 * have an index.
 * Red is in the list of properties but not required properties so should
 * return an index of -1.
 * Beige is not in the list of properties but is in required properties so 
 * should also return an index of -1.
 */
TEST_F(Properties, OneMissingProperty) {
	fiftyoneDegreesPropertiesRequired required;
	required.string = "Yellow,Beige";
	required.array = NULL;
	required.count = 0;
	required.existing = NULL;
	CreateProperties(&required);
	int reqIndex = fiftyoneDegreesPropertiesGetRequiredPropertyIndexFromName(
		this->properties,
		"Yellow");
	EXPECT_EQ(0, reqIndex);
	const char *name = FIFTYONE_DEGREES_STRING(
		fiftyoneDegreesPropertiesGetNameFromRequiredIndex(
			this->properties,
			reqIndex));
	EXPECT_STREQ("Yellow", name);
	EXPECT_EQ(-1, fiftyoneDegreesPropertiesGetRequiredPropertyIndexFromName(
		this->properties,
		"Beige"));
	EXPECT_EQ(-1,fiftyoneDegreesPropertiesGetRequiredPropertyIndexFromName(
		this->properties,
		"Red"));
}

/**
 * Check that passing a string list of required properties works as expected.
 * Yellow and Black are both in the list of properties and required properties 
 * so should have index values.
 * Since they are sorted alphabetically, Black should have index 0 and Yellow 
 * index 1.
 */
TEST_F(Properties, StringTwoPropertiesOrdered) {
	fiftyoneDegreesPropertiesRequired required;
	required.string = "Yellow,Black";
	required.array = NULL;
	required.count = 0;
	required.existing = NULL;
	CreateProperties(&required);
	EXPECT_EQ(0, fiftyoneDegreesPropertiesGetRequiredPropertyIndexFromName(
		this->properties,
		"Black"));
	EXPECT_EQ(1, fiftyoneDegreesPropertiesGetRequiredPropertyIndexFromName(
		this->properties,
		"Yellow"));
}

/**
 * Check that passing a string list of required properties works as expected.
 * Function should still work for other properties if there is a property
 * corrupted by a space.
 */
TEST_F(Properties, StringTwoPropertiesOrderedSpace) {
	fiftyoneDegreesPropertiesRequired required;
	required.string = "Cyan,Yellow, Black ,Blue|";
	required.array = NULL;
	required.count = 0;
	required.existing = NULL;
	CreateProperties(&required);
	EXPECT_EQ(0, fiftyoneDegreesPropertiesGetRequiredPropertyIndexFromName(
		this->properties,
		"Black"));
	EXPECT_EQ(1, fiftyoneDegreesPropertiesGetRequiredPropertyIndexFromName(
		this->properties,
		"Blue"));
	EXPECT_EQ(2, fiftyoneDegreesPropertiesGetRequiredPropertyIndexFromName(
		this->properties,
		"Yellow"));
}

/**
 * Check that passing an array of strings of required properties works as expected.
 * Yellow and Black are both in the list of properties and required properties 
 * so should have index values.
 * Since they are sorted alphabetically, Black should have index 0 and Yellow 
 * index 1.
 */
TEST_F(Properties, ArrayTwoPropertiesOrdered) {
	const char* tests[] = { "Yellow", "Black" };
	fiftyoneDegreesPropertiesRequired required;
	required.string = NULL;
	required.array = tests;
	required.count = sizeof(tests) / sizeof(const char*);
	required.existing = NULL;
	CreateProperties(&required);
	EXPECT_EQ(0, fiftyoneDegreesPropertiesGetRequiredPropertyIndexFromName(
		this->properties,
		"Black"));
	EXPECT_EQ(1, fiftyoneDegreesPropertiesGetRequiredPropertyIndexFromName(
		this->properties,
		"Yellow"));
}

/**
 * Check that passing an empty string works as expected. This should not throw
 * an error and should behave in the same way as if the string was null.
 */
TEST_F(Properties, StringEmpty) {
	fiftyoneDegreesPropertiesRequired required;
	required.string = "";
	required.array = NULL;
	required.count = 0;
	required.existing = NULL;
	CreateProperties(&required);
}

/**
 * Check that adding two identical properties in an array only yields results
 * with a single instance of the property.
 */
TEST_F(Properties, RepeatedArray) {
	const char* tests[]{ "Yellow", "Yellow" };
	fiftyoneDegreesPropertiesRequired required;
	required.string = NULL;
	required.array = tests;
	required.count = sizeof(tests) / sizeof(const char*);
	required.existing = NULL;
	CreateProperties(&required);
	ASSERT_EQ(1, this->properties->count);
}

/**
 * Check that adding two identical properties in a string only yields results
 * with a single instance of the property.
 */
TEST_F(Properties, RepeatedString) {
	const char* tests = "Yellow,Yellow";
	fiftyoneDegreesPropertiesRequired required;
	required.string = tests;
	required.array = NULL;
	required.count = 0;
	required.existing = NULL;
	CreateProperties(&required);
	ASSERT_EQ(1, this->properties->count);
}

/**
 * Check that adding two properties which are the same when case is ignored
 * only yields results with a single instance of the property.
 */
 TEST_F(Properties, RepeatedArray_DifferentCase) {
	const char* tests[]{ "yellow", "Yellow" };
	fiftyoneDegreesPropertiesRequired required;
	required.string = NULL;
	required.array = tests;
	required.count = sizeof(tests) / sizeof(const char*);
	required.existing = NULL;
	CreateProperties(&required);
	ASSERT_EQ(1, this->properties->count);
}

/**
 * Check that adding two properties which are the same when case is ignored
 * only yields results with a single instance of the property.
 */
TEST_F(Properties, RepeatedString_DifferentCase) {
	const char* tests = "yellow,Yellow";
	fiftyoneDegreesPropertiesRequired required;
	required.string = tests;
	required.array = NULL;
	required.count = 0;
	required.existing = NULL;
	CreateProperties(&required);
	ASSERT_EQ(1, this->properties->count);
}

/**
 * Check that case is not taken into account when fetching the required
 * property index i.e. get('property') == get('PROPERTY').
 */
TEST_F(Properties, CaseInsensitiveGetIndex) {
	const char* tests[]{ "yellow" };
	fiftyoneDegreesPropertiesRequired required;
	required.string = NULL;
	required.array = tests;
	required.count = sizeof(tests) / sizeof(const char*);
	required.existing = NULL;
	CreateProperties(&required);
	ASSERT_NE(
		-1,
		fiftyoneDegreesPropertiesGetRequiredPropertyIndexFromName(
			this->properties,
			"yellow"));
	ASSERT_EQ(
		fiftyoneDegreesPropertiesGetRequiredPropertyIndexFromName(
			this->properties,
			"Yellow"),
		fiftyoneDegreesPropertiesGetRequiredPropertyIndexFromName(
			this->properties,
			"yellow"));
}