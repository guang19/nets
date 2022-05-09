//
// Created by n021949 on 2022/5/7.
//

#include <gtest/gtest.h>

#include <endian.h>

bool isBigEndian()
{
	uint16_t var = 1;
	if (*((uint8_t*) &var) != 1)
	{
		return true;
	}
	return false;
}

bool isLittleEndian()
{
	uint16_t var = 1;
	if (*((uint8_t*) &var) == 1)
	{
		return true;
	}
	return false;
}

TEST(NetPackageTest, Endian)
{
	ASSERT_FALSE(isBigEndian());
	ASSERT_TRUE(isLittleEndian());
	uint32_t x = 1234;
	uint32_t y = htobe32(x);
	ASSERT_TRUE(x != y);
	uint32_t z = be32toh(y);
	ASSERT_EQ(x, z);
}

TEST(NetPackageTest, TT)
{
	printf("%ld", sysconf(_SC_NPROCESSORS_CONF));
	printf("%ld", sysconf(_SC_NPROCESSORS_ONLN));
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}