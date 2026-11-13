/*
*	OXWARE developed by oxiKKK
*	Copyright (c) 2023
* 
*	This program is licensed under the MIT license. By downloading, copying, 
*	installing or using this software you agree to this license.
*
*	License Agreement
*
*	Permission is hereby granted, free of charge, to any person obtaining a 
*	copy of this software and associated documentation files (the "Software"), 
*	to deal in the Software without restriction, including without limitation 
*	the rights to use, copy, modify, merge, publish, distribute, sublicense, 
*	and/or sell copies of the Software, and to permit persons to whom the 
*	Software is furnished to do so, subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included 
*	in all copies or substantial portions of the Software. 
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
*	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
*	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
*	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
*	IN THE SOFTWARE.
*/

#ifndef HLNETMESSAGEIO_H
#define HLNETMESSAGEIO_H
#pragma once


struct MSG_StartBitReadingFnHook_t final : public GenericMemoryFnHook_cdecl<void, hl::sizebuf_t*> { bool install() override; };


struct MSG_EndBitReadingFnHook_t final : public GenericMemoryFnHook_cdecl<void, hl::sizebuf_t*> { bool install() override; };


struct MSG_ReadBitsFnHook_t final : public GenericMemoryFnHook_cdecl<hl::uint32, int> { bool install() override; };


struct MSG_ReadSBitsFnHook_t final : public GenericMemoryFnHook_cdecl<hl::int32, int> { bool install() override; };


struct MSG_ReadBitCoordFnHook_t final : public GenericMemoryFnHook_cdecl<float> { bool install() override; };


struct MSG_ReadBitVec3CoordFnHook_t final : public GenericMemoryFnHook_cdecl<void, hl::vec_t*> { bool install() override; };


struct MSG_StartBitWritingFnHook_t final : public GenericMemoryFnHook_cdecl<void, hl::sizebuf_t*> { bool install() override; };


struct MSG_EndBitWritingFnHook_t final : public GenericMemoryFnHook_cdecl<void, hl::sizebuf_t*> { bool install() override; };


struct MSG_WriteBitsFnHook_t final : public GenericMemoryFnHook_cdecl<void, hl::uint32, int> { bool install() override; };


struct MSG_WriteBitDataFnHook_t final : public GenericMemoryFnHook_cdecl<void, void*, int> { bool install() override; };


struct MSG_ReadCharFnHook_t final : public GenericMemoryFnHook_cdecl<int> { bool install() override; };


struct MSG_ReadByteFnHook_t final : public GenericMemoryFnHook_cdecl<int> { bool install() override; };


struct MSG_ReadShortFnHook_t final : public GenericMemoryFnHook_cdecl<int> { bool install() override; };


struct MSG_ReadWordFnHook_t final : public GenericMemoryFnHook_cdecl<int> { bool install() override; };


struct MSG_ReadLongFnHook_t final : public GenericMemoryFnHook_cdecl<int> { bool install() override; };


struct MSG_ReadFloatFnHook_t final : public GenericMemoryFnHook_cdecl<float> { bool install() override; };


struct MSG_ReadStringFnHook_t final : public GenericMemoryFnHook_cdecl<char*> { bool install() override; };


struct MSG_ReadCoordFnHook_t final : public GenericMemoryFnHook_cdecl<float, hl::sizebuf_t*> { bool install() override; };


struct MSG_WriteCharFnHook_t final : public GenericMemoryFnHook_cdecl<void, hl::sizebuf_t*, int> { bool install() override; };


struct MSG_WriteByteFnHook_t final : public GenericMemoryFnHook_cdecl<void, hl::sizebuf_t*, int> { bool install() override; };


struct MSG_WriteShortFnHook_t final : public GenericMemoryFnHook_cdecl<void, hl::sizebuf_t*, int> { bool install() override; };


struct MSG_WriteWordFnHook_t final : public GenericMemoryFnHook_cdecl<void, hl::sizebuf_t*, int> { bool install() override; };


struct MSG_WriteLongFnHook_t final : public GenericMemoryFnHook_cdecl<void, hl::sizebuf_t*, int> { bool install() override; };


struct MSG_WriteFloatFnHook_t final : public GenericMemoryFnHook_cdecl<void, hl::sizebuf_t*, float> { bool install() override; };


struct MSG_WriteStringFnHook_t final : public GenericMemoryFnHook_cdecl<void, hl::sizebuf_t*, char*> { bool install() override; };


struct MSG_WriteCoordFnHook_t final : public GenericMemoryFnHook_cdecl<void, hl::sizebuf_t*, float> { bool install() override; };


struct MSG_WriteBufFnHook_t final : public GenericMemoryFnHook_cdecl<void, hl::sizebuf_t*, int, void*> { bool install() override; };


struct net_messageHook final : public GenericMemoryHook<hl::sizebuf_t> { bool install() override; };


struct msg_readcountHook final : public GenericMemoryHook<int> { bool install() override; };


struct bfreadHook final : public GenericMemoryHook<hl::bf_read_t> { bool install() override; };


class CHLNetMessageIO
{
public:
	DECL_BASIC_CLASS(CHLNetMessageIO);

public:
	bool install_hooks();


	bool is_ready() const
	{
		return m_initialized_hooks;
	}


	void begin_silent_bit_reading();
	void end_silent_bit_reading();
	uint32_t read_bits(int numbits);
	int32_t read_signed_bits(int numbits);
	float read_bit_coord();
	void read_bit_vec3coord(Vector& coord);


	void start_bit_writing(hl::sizebuf_t* sb);
	void end_bit_writing(hl::sizebuf_t* sb);
	void write_bits(uint32_t data, int num_bits);
	void write_bit_data(void* data, int num_bits);


	void begin_silent_reading();
	void end_silent_reading();
	void end_silent_reading_no_restore();
	int8_t read_int8();
	uint8_t read_uint8();
	int16_t read_int16();
	uint16_t read_uint16();
	int32_t read_int32();
	float read_float();
	char* read_string();
	float read_coord();


	void write_char(int8_t i8);
	void write_byte(uint8_t ui8);
	void write_short(int16_t i16);
	void write_word(uint16_t ui16);
	void write_long(int32_t i32);
	void write_float(float f);
	void write_string(const char* s);
	void write_coord(float coord);
	void write_buf(int iSize, void* buf);


	void write_char(hl::sizebuf_t* sb, int8_t i8);
	void write_byte(hl::sizebuf_t* sb, uint8_t ui8);
	void write_short(hl::sizebuf_t* sb, int16_t i16);
	void write_word(hl::sizebuf_t* sb, uint16_t ui16);
	void write_long(hl::sizebuf_t* sb, int32_t i32);
	void write_float(hl::sizebuf_t* sb, float f);
	void write_string(hl::sizebuf_t* sb, const char* s);
	void write_coord(hl::sizebuf_t* sb, float coord);
	void write_buf(hl::sizebuf_t* sb, int iSize, void* buf);

private:
	inline auto& net_message() { static net_messageHook hook; return hook; };
	inline auto& msg_readcount() { static msg_readcountHook hook; return hook; };
	inline auto& bfread() { static bfreadHook hook; return hook; };


	int m_num_read_saved = 0;
	bool m_in_read = false;

	void save_engine_internal_readcount() { m_num_read_saved = *msg_readcount().get(); }
	void restore_engine_internal_readcount() { *msg_readcount().get() = m_num_read_saved; }


	hl::bf_read_t m_saved_read_state = {};
	bool m_in_bit_read = false;

	void save_engine_internal_bitreadstate() 
	{
		memcpy(&m_saved_read_state, bfread().get(), sizeof(hl::bf_read_t));
	}

	void restore_engine_internal_bitreadstate()
	{
		memcpy(bfread().get(), &m_saved_read_state, sizeof(hl::bf_read_t));
	}


	bool m_initialized_hooks = false;

private:

	inline auto& MSG_StartBitReading() { static MSG_StartBitReadingFnHook_t hook; return hook; }
	inline auto& MSG_EndBitReading() { static MSG_EndBitReadingFnHook_t hook; return hook; }
	inline auto& MSG_ReadBits() { static MSG_ReadBitsFnHook_t hook; return hook; }
	inline auto& MSG_ReadSBits() { static MSG_ReadSBitsFnHook_t hook; return hook; }
	inline auto& MSG_ReadBitCoord() { static MSG_ReadBitCoordFnHook_t hook; return hook; }
	inline auto& MSG_ReadBitVec3Coord() { static MSG_ReadBitVec3CoordFnHook_t hook; return hook; }


	inline auto& MSG_StartBitWriting() { static MSG_StartBitWritingFnHook_t hook; return hook; }
	inline auto& MSG_EndBitWriting() { static MSG_EndBitWritingFnHook_t hook; return hook; }
	inline auto& MSG_WriteBits() { static MSG_WriteBitsFnHook_t hook; return hook; }
	inline auto& MSG_WriteBitData() { static MSG_WriteBitDataFnHook_t hook; return hook; }


	inline auto& MSG_ReadChar() { static MSG_ReadCharFnHook_t hook; return hook; }
	inline auto& MSG_ReadByte() { static MSG_ReadByteFnHook_t hook; return hook; }
	inline auto& MSG_ReadShort() { static MSG_ReadShortFnHook_t hook; return hook; }
	inline auto& MSG_ReadWord() { static MSG_ReadWordFnHook_t hook; return hook; }
	inline auto& MSG_ReadLong() { static MSG_ReadLongFnHook_t hook; return hook; }
	inline auto& MSG_ReadFloat() { static MSG_ReadFloatFnHook_t hook; return hook; }
	inline auto& MSG_ReadString() { static MSG_ReadStringFnHook_t hook; return hook; }
	inline auto& MSG_ReadCoord() { static MSG_ReadCoordFnHook_t hook; return hook; }


	inline auto& MSG_WriteChar() { static MSG_WriteCharFnHook_t hook; return hook; }
	inline auto& MSG_WriteByte() { static MSG_WriteByteFnHook_t hook; return hook; }
	inline auto& MSG_WriteShort() { static MSG_WriteShortFnHook_t hook; return hook; }
	inline auto& MSG_WriteWord() { static MSG_WriteWordFnHook_t hook; return hook; }
	inline auto& MSG_WriteLong() { static MSG_WriteLongFnHook_t hook; return hook; }
	inline auto& MSG_WriteFloat() { static MSG_WriteFloatFnHook_t hook; return hook; }
	inline auto& MSG_WriteString() { static MSG_WriteStringFnHook_t hook; return hook; }
	inline auto& MSG_WriteCoord() { static MSG_WriteCoordFnHook_t hook; return hook; }
	inline auto& MSG_WriteBuf() { static MSG_WriteBufFnHook_t hook; return hook; }
};

#endif