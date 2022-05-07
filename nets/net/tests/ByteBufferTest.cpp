//
// Created by n021949 on 2022/5/7.
//

#include <gtest/gtest.h>

#include <endian.h>

bool isBigEndian()
{
	uint16_t var = 1;
	if (*((uint8_t*)&var) != 1)
	{
		return true;
	}
	return false;
}

bool isLittleEndian()
{
	uint16_t var = 1;
	if (*((uint8_t*)&var) == 1)
	{
		return true;
	}
	return false;
}

TEST(ByteBufferTest, Endian)
{
	ASSERT_FALSE(isBigEndian());
	ASSERT_TRUE(isLittleEndian());
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}