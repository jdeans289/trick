#include <gtest/gtest.h>
#include <iostream>
#include <iomanip>
#include <limits>

#include "trick/MemoryManager.hh"
#include "trick/VariableReference.hh"
#include "trick/UdUnits.hh"

#include "TestObject.hh"

/*
 Test Fixture.
 */
class VariableReference_test : public ::testing::Test {
	protected:
	    Trick::MemoryManager *memmgr;
        Trick::UdUnits * udunits;
		VariableReference_test() { 
            memmgr = new Trick::MemoryManager; 
            udunits = new Trick::UdUnits; 

            udunits->read_default_xml();
        }
		~VariableReference_test() { delete memmgr; }
		void SetUp() {}
		void TearDown() {}
};

TEST_F(VariableReference_test, getName) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_int = 5;
    (void) memmgr->declare_extern_var(&test_int, "int test_int");
    Trick::VariableReference ref("test_int");

    // ACT

    // ASSERT
    EXPECT_EQ(strcmp(ref.getName(), "test_int"), 0);
}

TEST_F(VariableReference_test, validateAddress) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");
    
    float test_b = 5.0;
    (void) memmgr->declare_extern_var(&test_b, "float test_b");
    Trick::VariableReference ref_broken("test_b");
    memmgr->delete_var("test_b");

    float test_c = 5.0;
    (void) memmgr->declare_extern_var(&test_c, "float test_c");
    Trick::VariableReference ref_tagged("test_c");
    ref_tagged.tagAsInvalid();

    // ACT
    // ASSERT
    EXPECT_EQ(ref.validate(), true);
    EXPECT_EQ(ref_broken.validate(), false);
    // A reference already tagged as invalid is considered ok by this check
    EXPECT_EQ(ref_tagged.validate(), true);
}


TEST_F(VariableReference_test, stageValue_set) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");

    // ACT
    ref.stageValue();

    // ASSERT
    EXPECT_EQ(ref.isStaged(), true);
    EXPECT_EQ(ref.isWriteReady(), false);
}

TEST_F(VariableReference_test, prepareForWrite_set) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");

    // ACT
    ref.stageValue();
    ref.prepareForWrite();

    // ASSERT
    EXPECT_EQ(ref.isStaged(), false);
    EXPECT_EQ(ref.isWriteReady(), true);
}

TEST_F(VariableReference_test, prepareForWrite_fails_if_not_staged) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");

    // ACT
    ref.prepareForWrite();

    // ASSERT
    EXPECT_EQ(ref.isStaged(), false);
    EXPECT_EQ(ref.isWriteReady(), false);
}

TEST_F(VariableReference_test, writeValueAscii_fails_if_not_write_ready) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    // ASSERT
    EXPECT_EQ(ref.writeValueAscii(ss), -1);
}

TEST_F(VariableReference_test, writeValueAscii_int) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 5;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "5");
}

TEST_F(VariableReference_test, writeValueAscii_int_arr) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a[5] = {1, 2, 3, 4, 5};
    (void) memmgr->declare_extern_var(&test_a, "int test_a[5]");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "1,2,3,4,5");
}

TEST_F(VariableReference_test, writeValueAscii_double) {
    // ARRANGE
    // Create a variable to make a reference for
    double test_a = 867.309;
    (void) memmgr->declare_extern_var(&test_a, "double test_a");
    Trick::VariableReference ref_a("test_a");
    std::stringstream ss_a;

    double test_b = std::numeric_limits<double>::max();
    (void) memmgr->declare_extern_var(&test_b, "double test_b");
    Trick::VariableReference ref_b("test_b");
    std::stringstream ss_b;

    double test_c = std::numeric_limits<double>::min();
    (void) memmgr->declare_extern_var(&test_c, "double test_c");
    Trick::VariableReference ref_c("test_c");
    std::stringstream ss_c;

    // ACT
    ref_a.stageValue();
    ref_a.prepareForWrite();
    ref_a.writeValueAscii(ss_a);

    ref_b.stageValue();
    ref_b.prepareForWrite();
    ref_b.writeValueAscii(ss_b);

    ref_c.stageValue();
    ref_c.prepareForWrite();
    ref_c.writeValueAscii(ss_c);

    // ASSERT
    EXPECT_EQ(ss_a.str(), "867.309");
    EXPECT_EQ(ss_b.str(), "1.797693134862316e+308");
    EXPECT_EQ(ss_c.str(), "2.225073858507201e-308");
}

TEST_F(VariableReference_test, writeValueAscii_char) {
    // ARRANGE
    // Create a variable to make a reference for
    char test_a = 'j';
    (void) memmgr->declare_extern_var(&test_a, "char test_a");
    Trick::VariableReference ref_a("test_a");
    std::stringstream ssa;

    char test_b[7] = "jackie";
    (void) memmgr->declare_extern_var(&test_b, "char test_b[7]");
    Trick::VariableReference ref_b("test_b");
    std::stringstream ssb;

    // ACT
    ref_a.stageValue();
    ref_b.stageValue();

    ref_a.prepareForWrite();
    ref_b.prepareForWrite();

    ref_a.writeValueAscii(ssa);
    ref_b.writeValueAscii(ssb);


    // ASSERT
    EXPECT_EQ(ssa.str(), "j");
    EXPECT_EQ(ssb.str(), "jackie");
}


TEST_F(VariableReference_test, writeValueAscii_unsigned_char) {
    // ARRANGE
    // Create a variable to make a reference for
    unsigned char test_a = 'j';
    (void) memmgr->declare_extern_var(&test_a, "unsigned char test_a");
    Trick::VariableReference ref_a("test_a");
    std::stringstream ssa;

    unsigned char test_b[8] = "jackie\n";
    (void) memmgr->declare_extern_var(&test_b, "unsigned char test_b[8]");
    Trick::VariableReference ref_b("test_b");
    std::stringstream ssb;

    // ACT
    ref_a.stageValue();
    ref_b.stageValue();

    ref_a.prepareForWrite();
    ref_b.prepareForWrite();

    ref_a.writeValueAscii(ssa);
    ref_b.writeValueAscii(ssb);

    // ASSERT
    EXPECT_EQ(ssa.str(), "j");
    EXPECT_EQ(ssb.str(), "jackie\\n");
}


TEST_F(VariableReference_test, writeValueAscii_wide_char) {
    // ARRANGE
    // Create a variable to make a reference for
    wchar_t test_a = L'J';
    (void) memmgr->declare_extern_var(&test_a, "wchar_t test_a");
    Trick::VariableReference ref_a("test_a");
    std::stringstream ssa;

    wchar_t test_b[15] = L"jackiebutwider";
    (void) memmgr->declare_extern_var(&test_b, "wchar_t test_b[15]");
    Trick::VariableReference ref_b("test_b");
    std::stringstream ssb;

   // ACT
    ref_a.stageValue();
    ref_b.stageValue();

    ref_a.prepareForWrite();
    ref_b.prepareForWrite();

    ref_a.writeValueAscii(ssa);
    ref_b.writeValueAscii(ssb);

    // ASSERT
    // Original variable server behavior prints individual wchar as its ascii value
    EXPECT_EQ(ssa.str(), "74");
    EXPECT_EQ(ssb.str(), "jackiebutwider");
}

TEST_F(VariableReference_test, writeValueAscii_std_string) {
    // ARRANGE
    // Create a variable to make a reference for
    std::string test_a = "jackiebutstringy";
    (void) memmgr->declare_extern_var(&test_a, "std::string test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "jackiebutstringy");
}

TEST_F(VariableReference_test, writeValueAscii_escape_characters) {
    // ARRANGE
    // Create a variable to make a reference for
    std::string test_a = "\n\t\b\a\"\f\r\v";
    (void) memmgr->declare_extern_var(&test_a, "std::string test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "\\n\\t\\b\\a\"\\f\\n\\v");
}

TEST_F(VariableReference_test, writeValueAscii_boolean) {
    // ARRANGE
    // Create a variable to make a reference for
    bool test_a = true;
    (void) memmgr->declare_extern_var(&test_a, "bool test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "1");
}

TEST_F(VariableReference_test, writeValueAscii_short) {
    // ARRANGE
    // Create a variable to make a reference for
    short test_a = 255;
    (void) memmgr->declare_extern_var(&test_a, "short test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);

    // ASSERT
    EXPECT_EQ(ss.str(), "255");
}

TEST_F(VariableReference_test, printWithoutUnits) {
    // ARRANGE
    // Create a variable to make a reference for
    TestObject obj;
    obj.length = 5000;
    (void) memmgr->declare_extern_var(&obj, "TestObject obj");
    Trick::VariableReference ref("obj.length");
    std::stringstream ss;

    // ACT
    EXPECT_STREQ(ref.getBaseUnits(), "m");

    // ASSERT
    // Doesn't actually print with units unless set
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);
    EXPECT_EQ(ss.str(), "5000");
}

TEST_F(VariableReference_test, setUnits) {
    // ARRANGE
    // Create a variable to make a reference for
    TestObject obj;
    obj.length = 5000;
    (void) memmgr->declare_extern_var(&obj, "TestObject obj");
    Trick::VariableReference ref("obj.length");
    std::stringstream ss;

    // ACT
    ref.setRequestedUnits("km");

    // ASSERT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);
    EXPECT_EQ(ss.str(), "5 {km}");
}

TEST_F(VariableReference_test, setUnitsTwice) {
    // ARRANGE
    // Create a variable to make a reference for
    TestObject obj;
    obj.length = 5000;
    (void) memmgr->declare_extern_var(&obj, "TestObject obj");
    Trick::VariableReference ref("obj.length");
    std::stringstream ss;

    // ACT
    ref.setRequestedUnits("km");

    // ASSERT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);
    EXPECT_EQ(ss.str(), "5 {km}");
    ss.str("");

    // ACT
    ref.setRequestedUnits("mm");

    // ASSERT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueAscii(ss);
    EXPECT_EQ(ss.str(), "5000000 {mm}");
}

TEST_F(VariableReference_test, writeValueBinary_int) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 4095;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueBinary(ss);

    
    unsigned char expected_bytes[4] = {0xFF, 0x0F, 0x00, 0x00};
    char * actual_bytes = (char *) malloc (sizeof(int));

    ss.read(actual_bytes, 4);

    // ASSERT
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(static_cast<unsigned char>(actual_bytes[i]), expected_bytes[i]);
    }
}

TEST_F(VariableReference_test, writeValueBinary_float) {
    // ARRANGE
    // Create a variable to make a reference for
    float test_a = 40.95;
    (void) memmgr->declare_extern_var(&test_a, "float test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueBinary(ss);

    
    // unsigned char expected_bytes[4] = {0x42, 0x23, 0xcc, 0xcd};
    unsigned char expected_bytes[4] = {0xcd, 0xcc, 0x23, 0x42};

    char * actual_bytes = (char *) malloc (sizeof(float));

    ss.read(actual_bytes, 4);

    // ASSERT
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(static_cast<unsigned char>(actual_bytes[i]), expected_bytes[i]);
    }
}

TEST_F(VariableReference_test, writeValueBinary_int_arr) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a[3] = {1, 2, 3};
    (void) memmgr->declare_extern_var(&test_a, "int test_a[3]");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueBinary(ss);

    // ASSERT
    char * actual_bytes = (char *) malloc (sizeof(int )* 3);
    ss.read(actual_bytes, sizeof(int) * 3);
    unsigned char expected_bytes[12] = {0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00};

    std::cout << "Received bytes: " << std::endl;
    for (int i = 0; i < 12; i++) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')  << (0xFF & actual_bytes[i]) << " ";
    }
    std::cout << std::endl;

    // ASSERT
    for (int i = 0; i < 12; i++) {
        EXPECT_EQ(static_cast<unsigned char>(actual_bytes[i]), expected_bytes[i]);
    }
}


TEST_F(VariableReference_test, writeValueBinary_double_arr) {
    // ARRANGE
    // Create a variable to make a reference for
    double test_a[3] = {1.0, 10.0, 100.0};
    (void) memmgr->declare_extern_var(&test_a, "double test_a[3]");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueBinary(ss);

    // ASSERT
    char * actual_bytes = (char *) malloc (sizeof(double )* 3);
    ss.read(actual_bytes, sizeof(double) * 3);
    unsigned char expected_bytes[24] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x59, 0x40};

    // std::cout << "Received bytes: " << std::endl;
    // for (int i = 0; i < sizeof(double) * 3; i++) {
    //     std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')  << (0xFF & actual_bytes[i]) << ", ";
    // }
    // std::cout << std::endl;

    // ASSERT
    for (int i = 0; i < 12; i++) {
        EXPECT_EQ(static_cast<unsigned char>(actual_bytes[i]), expected_bytes[i]);
    }
}


TEST_F(VariableReference_test, writeValueBinary_string) {
    // ARRANGE
    // Create a variable to make a reference for
    std::string test_a = "abcdef";
    (void) memmgr->declare_extern_var(&test_a, "std::string test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueBinary(ss);

    // ASSERT
    char * actual_bytes = (char *) malloc (test_a.length());
    ss.read(actual_bytes, test_a.length());
    unsigned char expected_bytes[6] = {0x61, 0x62, 0x63, 0x64, 0x65, 0x66};

    std::cout << "Received bytes: " << std::endl;
    for (int i = 0; i < test_a.length(); i++) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')  << (0xFF & actual_bytes[i]) << ", ";
    }
    std::cout << std::endl;

    // ASSERT
    for (int i = 0; i < test_a.length(); i++) {
        EXPECT_EQ(static_cast<unsigned char>(actual_bytes[i]), expected_bytes[i]);
    }
}

TEST_F(VariableReference_test, writeNameBinary) {
    // ARRANGE
    // Create a variable to make a reference for
    std::string test_a = "abcdef";
    (void) memmgr->declare_extern_var(&test_a, "std::string test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeNameBinary(ss);

    // ASSERT
    char * actual_bytes = (char *) malloc (sizeof(int) + strlen(ref.getName()));
    ss.read(actual_bytes, sizeof(int) +  6);
    unsigned char expected_bytes[sizeof(int) +  6] = {0x06, 0x00, 0x00, 0x00, 0x74, 0x65, 0x73, 0x74, 0x5f, 0x61};

    std::cout << "Received bytes: " << std::endl;
    for (int i = 0; i < sizeof(int) +  strlen(ref.getName()); i++) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')  << (0xFF & actual_bytes[i]) << ", ";
    }
    std::cout << std::endl;

    // ASSERT
    for (int i = 0; i < sizeof(int) +  strlen(ref.getName()); i++) {
        EXPECT_EQ(static_cast<unsigned char>(actual_bytes[i]), expected_bytes[i]);
    }
}


TEST_F(VariableReference_test, writeValueBinaryByteswap_int) {
    // ARRANGE
    // Create a variable to make a reference for
    int test_a = 4095;
    (void) memmgr->declare_extern_var(&test_a, "int test_a");
    Trick::VariableReference ref("test_a");
    std::stringstream ss;

    // ACT
    ref.stageValue();
    ref.prepareForWrite();
    ref.writeValueBinary(ss, true);

    
    unsigned char expected_bytes[4] = {0x00, 0x00, 0x0F, 0xFF};
    char * actual_bytes = (char *) malloc (sizeof(int));

    ss.read(actual_bytes, 4);

    // ASSERT
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(static_cast<unsigned char>(actual_bytes[i]), expected_bytes[i]);
    }
}