// Copyright 2011 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <stdlib.h>

#include "src/v8.h"

#include "src/code-factory.h"
#include "src/debug/debug.h"
#include "src/disasm.h"
#include "src/disassembler.h"
#include "src/ia32/frames-ia32.h"
#include "src/macro-assembler.h"
#include "test/cctest/cctest.h"

using namespace v8::internal;


#define __ assm.


static void DummyStaticFunction(Object* result) {
}


TEST(DisasmIa320) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  v8::internal::byte buffer[4096];
  Assembler assm(isolate, buffer, sizeof buffer);
  DummyStaticFunction(NULL);  // just bloody use it (DELETE; debugging)

  // Short immediate instructions
  __ adc(eax, 12345678);
  __ add(eax, Immediate(12345678));
  __ or_(eax, 12345678);
  __ sub(eax, Immediate(12345678));
  __ xor_(eax, 12345678);
  __ and_(eax, 12345678);
  Handle<FixedArray> foo = isolate->factory()->NewFixedArray(10, TENURED);
  __ cmp(eax, foo);

  // ---- This one caused crash
  __ mov(ebx,  Operand(esp, ecx, times_2, 0));  // [esp+ecx*4]

  // ---- All instructions that I can think of
  __ add(edx, ebx);
  __ add(edx, Operand(12, RelocInfo::NONE32));
  __ add(edx, Operand(ebx, 0));
  __ add(edx, Operand(ebx, 16));
  __ add(edx, Operand(ebx, 1999));
  __ add(edx, Operand(ebx, -4));
  __ add(edx, Operand(ebx, -1999));
  __ add(edx, Operand(esp, 0));
  __ add(edx, Operand(esp, 16));
  __ add(edx, Operand(esp, 1999));
  __ add(edx, Operand(esp, -4));
  __ add(edx, Operand(esp, -1999));
  __ nop();
  __ add(esi, Operand(ecx, times_4, 0));
  __ add(esi, Operand(ecx, times_4, 24));
  __ add(esi, Operand(ecx, times_4, -4));
  __ add(esi, Operand(ecx, times_4, -1999));
  __ nop();
  __ add(edi, Operand(ebp, ecx, times_4, 0));
  __ add(edi, Operand(ebp, ecx, times_4, 12));
  __ add(edi, Operand(ebp, ecx, times_4, -8));
  __ add(edi, Operand(ebp, ecx, times_4, -3999));
  __ add(Operand(ebp, ecx, times_4, 12), Immediate(12));

  __ nop();
  __ add(ebx, Immediate(12));
  __ nop();
  __ adc(edx, Operand(ebx));
  __ adc(ecx, 12);
  __ adc(ecx, 1000);
  __ nop();
  __ and_(edx, 3);
  __ and_(edx, Operand(esp, 4));
  __ cmp(edx, 3);
  __ cmp(edx, Operand(esp, 4));
  __ cmp(Operand(ebp, ecx, times_4, 0), Immediate(1000));
  Handle<FixedArray> foo2 = isolate->factory()->NewFixedArray(10, TENURED);
  __ cmp(ebx, foo2);
  __ cmpb(ebx, Operand(ebp, ecx, times_2, 0));
  __ cmpb(Operand(ebp, ecx, times_2, 0), ebx);
  __ or_(edx, 3);
  __ xor_(edx, 3);
  __ nop();
  __ cpuid();
  __ movsx_b(edx, ecx);
  __ movsx_w(edx, ecx);
  __ movzx_b(edx, ecx);
  __ movzx_w(edx, ecx);

  __ nop();
  __ imul(edx, ecx);
  __ shld(edx, ecx, 10);
  __ shld_cl(edx, ecx);
  __ shrd(edx, ecx, 10);
  __ shrd_cl(edx, ecx);
  __ bts(edx, ecx);
  __ bts(Operand(ebx, ecx, times_4, 0), ecx);
  __ nop();
  __ pushad();
  __ popad();
  __ pushfd();
  __ popfd();
  __ push(Immediate(12));
  __ push(Immediate(23456));
  __ push(ecx);
  __ push(esi);
  __ push(Operand(ebp, JavaScriptFrameConstants::kFunctionOffset));
  __ push(Operand(ebx, ecx, times_4, 0));
  __ push(Operand(ebx, ecx, times_4, 0));
  __ push(Operand(ebx, ecx, times_4, 10000));
  __ pop(edx);
  __ pop(eax);
  __ pop(Operand(ebx, ecx, times_4, 0));
  __ nop();

  __ add(edx, Operand(esp, 16));
  __ add(edx, ecx);
  __ mov_b(edx, ecx);
  __ mov_b(ecx, 6);
  __ mov_b(Operand(ebx, ecx, times_4, 10000), 6);
  __ mov_b(Operand(esp, 16), edx);
  __ mov_w(edx, Operand(esp, 16));
  __ mov_w(Operand(esp, 16), edx);
  __ nop();
  __ movsx_w(edx, Operand(esp, 12));
  __ movsx_b(edx, Operand(esp, 12));
  __ movzx_w(edx, Operand(esp, 12));
  __ movzx_b(edx, Operand(esp, 12));
  __ nop();
  __ mov(edx, 1234567);
  __ mov(edx, Operand(esp, 12));
  __ mov(Operand(ebx, ecx, times_4, 10000), Immediate(12345));
  __ mov(Operand(ebx, ecx, times_4, 10000), edx);
  __ nop();
  __ dec_b(edx);
  __ dec_b(Operand(eax, 10));
  __ dec_b(Operand(ebx, ecx, times_4, 10000));
  __ dec(edx);
  __ cdq();

  __ nop();
  __ idiv(edx);
  __ idiv(Operand(edx, ecx, times_1, 1));
  __ idiv(Operand(esp, 12));
  __ div(edx);
  __ div(Operand(edx, ecx, times_1, 1));
  __ div(Operand(esp, 12));
  __ mul(edx);
  __ neg(edx);
  __ not_(edx);
  __ test(Operand(ebx, ecx, times_4, 10000), Immediate(123456));

  __ imul(edx, Operand(ebx, ecx, times_4, 10000));
  __ imul(edx, ecx, 12);
  __ imul(edx, Operand(edx, eax, times_2, 42), 8);
  __ imul(edx, ecx, 1000);
  __ imul(edx, Operand(ebx, ecx, times_4, 1), 9000);

  __ inc(edx);
  __ inc(Operand(ebx, ecx, times_4, 10000));
  __ push(Operand(ebx, ecx, times_4, 10000));
  __ pop(Operand(ebx, ecx, times_4, 10000));
  __ call(Operand(ebx, ecx, times_4, 10000));
  __ jmp(Operand(ebx, ecx, times_4, 10000));

  __ lea(edx, Operand(ebx, ecx, times_4, 10000));
  __ or_(edx, 12345);
  __ or_(edx, Operand(ebx, ecx, times_4, 10000));

  __ nop();

  __ rcl(edx, 1);
  __ rcl(edx, 7);
  __ rcr(edx, 1);
  __ rcr(edx, 7);
  __ ror(edx, 1);
  __ ror(edx, 6);
  __ ror_cl(edx);
  __ ror(Operand(ebx, ecx, times_4, 10000), 1);
  __ ror(Operand(ebx, ecx, times_4, 10000), 6);
  __ ror_cl(Operand(ebx, ecx, times_4, 10000));
  __ sar(edx, 1);
  __ sar(edx, 6);
  __ sar_cl(edx);
  __ sar(Operand(ebx, ecx, times_4, 10000), 1);
  __ sar(Operand(ebx, ecx, times_4, 10000), 6);
  __ sar_cl(Operand(ebx, ecx, times_4, 10000));
  __ sbb(edx, Operand(ebx, ecx, times_4, 10000));
  __ shl(edx, 1);
  __ shl(edx, 6);
  __ shl_cl(edx);
  __ shl(Operand(ebx, ecx, times_4, 10000), 1);
  __ shl(Operand(ebx, ecx, times_4, 10000), 6);
  __ shl_cl(Operand(ebx, ecx, times_4, 10000));
  __ shrd_cl(Operand(ebx, ecx, times_4, 10000), edx);
  __ shr(edx, 1);
  __ shr(edx, 7);
  __ shr_cl(edx);
  __ shr(Operand(ebx, ecx, times_4, 10000), 1);
  __ shr(Operand(ebx, ecx, times_4, 10000), 6);
  __ shr_cl(Operand(ebx, ecx, times_4, 10000));


  // Immediates

  __ adc(edx, 12345);

  __ add(ebx, Immediate(12));
  __ add(Operand(edx, ecx, times_4, 10000), Immediate(12));

  __ and_(ebx, 12345);

  __ cmp(ebx, 12345);
  __ cmp(ebx, Immediate(12));
  __ cmp(Operand(edx, ecx, times_4, 10000), Immediate(12));
  __ cmpb(eax, Immediate(100));

  __ or_(ebx, 12345);

  __ sub(ebx, Immediate(12));
  __ sub(Operand(edx, ecx, times_4, 10000), Immediate(12));

  __ xor_(ebx, 12345);

  __ imul(edx, ecx, 12);
  __ imul(edx, ecx, 1000);

  __ cld();
  __ rep_movs();
  __ rep_stos();
  __ stos();

  __ sub(edx, Operand(ebx, ecx, times_4, 10000));
  __ sub(edx, ebx);

  __ test(edx, Immediate(12345));
  __ test(edx, Operand(ebx, ecx, times_8, 10000));
  __ test(Operand(esi, edi, times_1, -20000000), Immediate(300000000));
  __ test_b(edx, Operand(ecx, ebx, times_2, 1000));
  __ test_b(Operand(eax, -20), Immediate(0x9A));
  __ nop();

  __ xor_(edx, 12345);
  __ xor_(edx, Operand(ebx, ecx, times_8, 10000));
  __ bts(Operand(ebx, ecx, times_8, 10000), edx);
  __ hlt();
  __ int3();
  __ ret(0);
  __ ret(8);

  // Calls

  Label L1, L2;
  __ bind(&L1);
  __ nop();
  __ call(&L1);
  __ call(&L2);
  __ nop();
  __ bind(&L2);
  __ call(Operand(ebx, ecx, times_4, 10000));
  __ nop();
  Handle<Code> ic(CodeFactory::LoadIC(isolate).code());
  __ call(ic, RelocInfo::CODE_TARGET);
  __ nop();
  __ call(FUNCTION_ADDR(DummyStaticFunction), RelocInfo::RUNTIME_ENTRY);
  __ nop();

  __ jmp(&L1);
  __ jmp(Operand(ebx, ecx, times_4, 10000));
  ExternalReference after_break_target =
      ExternalReference::debug_after_break_target_address(isolate);
  __ jmp(Operand::StaticVariable(after_break_target));
  __ jmp(ic, RelocInfo::CODE_TARGET);
  __ nop();


  Label Ljcc;
  __ nop();
  // long jumps
  __ j(overflow, &Ljcc);
  __ j(no_overflow, &Ljcc);
  __ j(below, &Ljcc);
  __ j(above_equal, &Ljcc);
  __ j(equal, &Ljcc);
  __ j(not_equal, &Ljcc);
  __ j(below_equal, &Ljcc);
  __ j(above, &Ljcc);
  __ j(sign, &Ljcc);
  __ j(not_sign, &Ljcc);
  __ j(parity_even, &Ljcc);
  __ j(parity_odd, &Ljcc);
  __ j(less, &Ljcc);
  __ j(greater_equal, &Ljcc);
  __ j(less_equal, &Ljcc);
  __ j(greater, &Ljcc);
  __ nop();
  __ bind(&Ljcc);
  // short jumps
  __ j(overflow, &Ljcc);
  __ j(no_overflow, &Ljcc);
  __ j(below, &Ljcc);
  __ j(above_equal, &Ljcc);
  __ j(equal, &Ljcc);
  __ j(not_equal, &Ljcc);
  __ j(below_equal, &Ljcc);
  __ j(above, &Ljcc);
  __ j(sign, &Ljcc);
  __ j(not_sign, &Ljcc);
  __ j(parity_even, &Ljcc);
  __ j(parity_odd, &Ljcc);
  __ j(less, &Ljcc);
  __ j(greater_equal, &Ljcc);
  __ j(less_equal, &Ljcc);
  __ j(greater, &Ljcc);

  // 0xD9 instructions
  __ nop();

  __ fld(1);
  __ fld1();
  __ fldz();
  __ fldpi();
  __ fabs();
  __ fchs();
  __ fprem();
  __ fprem1();
  __ fincstp();
  __ ftst();
  __ fxch(3);
  __ fld_s(Operand(ebx, ecx, times_4, 10000));
  __ fstp_s(Operand(ebx, ecx, times_4, 10000));
  __ ffree(3);
  __ fld_d(Operand(ebx, ecx, times_4, 10000));
  __ fstp_d(Operand(ebx, ecx, times_4, 10000));
  __ nop();

  __ fild_s(Operand(ebx, ecx, times_4, 10000));
  __ fistp_s(Operand(ebx, ecx, times_4, 10000));
  __ fild_d(Operand(ebx, ecx, times_4, 10000));
  __ fistp_d(Operand(ebx, ecx, times_4, 10000));
  __ fnstsw_ax();
  __ nop();
  __ fadd(3);
  __ fsub(3);
  __ fmul(3);
  __ fdiv(3);

  __ faddp(3);
  __ fsubp(3);
  __ fmulp(3);
  __ fdivp(3);
  __ fcompp();
  __ fwait();
  __ frndint();
  __ fninit();
  __ nop();

  // SSE instruction
  {
    // Move operation
    __ movaps(xmm0, xmm1);
    __ shufps(xmm0, xmm0, 0x0);
    __ cvtsd2ss(xmm0, xmm1);
    __ cvtsd2ss(xmm0, Operand(ebx, ecx, times_4, 10000));

    // logic operation
    __ andps(xmm0, xmm1);
    __ andps(xmm0, Operand(ebx, ecx, times_4, 10000));
    __ orps(xmm0, xmm1);
    __ orps(xmm0, Operand(ebx, ecx, times_4, 10000));
    __ xorps(xmm0, xmm1);
    __ xorps(xmm0, Operand(ebx, ecx, times_4, 10000));

    // Arithmetic operation
    __ addss(xmm1, xmm0);
    __ addss(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ mulss(xmm1, xmm0);
    __ mulss(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ subss(xmm1, xmm0);
    __ subss(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ divss(xmm1, xmm0);
    __ divss(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ maxss(xmm1, xmm0);
    __ maxss(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ minss(xmm1, xmm0);
    __ minss(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ sqrtss(xmm1, xmm0);
    __ sqrtss(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ addps(xmm1, xmm0);
    __ addps(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ subps(xmm1, xmm0);
    __ subps(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ mulps(xmm1, xmm0);
    __ mulps(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ divps(xmm1, xmm0);
    __ divps(xmm1, Operand(ebx, ecx, times_4, 10000));

    __ ucomiss(xmm0, xmm1);
    __ ucomiss(xmm0, Operand(ebx, ecx, times_4, 10000));
  }
  {
    __ cvttss2si(edx, Operand(ebx, ecx, times_4, 10000));
    __ cvtsi2sd(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ cvtss2sd(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ cvtss2sd(xmm1, xmm0);
    __ movsd(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ movsd(Operand(ebx, ecx, times_4, 10000), xmm1);
    // 128 bit move instructions.
    __ movdqa(xmm0, Operand(ebx, ecx, times_4, 10000));
    __ movdqa(Operand(ebx, ecx, times_4, 10000), xmm0);
    __ movdqu(xmm0, Operand(ebx, ecx, times_4, 10000));
    __ movdqu(Operand(ebx, ecx, times_4, 10000), xmm0);

    __ addsd(xmm1, xmm0);
    __ addsd(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ mulsd(xmm1, xmm0);
    __ mulsd(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ subsd(xmm1, xmm0);
    __ subsd(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ divsd(xmm1, xmm0);
    __ divsd(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ minsd(xmm1, xmm0);
    __ minsd(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ maxsd(xmm1, xmm0);
    __ maxsd(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ ucomisd(xmm0, xmm1);
    __ cmpltsd(xmm0, xmm1);

    __ andpd(xmm0, xmm1);
    __ psllq(xmm0, 17);
    __ psllq(xmm0, xmm1);
    __ psrlq(xmm0, 17);
    __ psrlq(xmm0, xmm1);
    __ por(xmm0, xmm1);

    __ pcmpeqd(xmm1, xmm0);

    __ punpckldq(xmm1, xmm6);
    __ punpckhdq(xmm7, xmm5);
  }
  {
    __ cvttss2si(edx, Operand(ebx, ecx, times_4, 10000));
    __ cvtsi2sd(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ movsd(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ movsd(Operand(ebx, ecx, times_4, 10000), xmm1);
    // 128 bit move instructions.
    __ movdqa(xmm0, Operand(ebx, ecx, times_4, 10000));
    __ movdqa(Operand(ebx, ecx, times_4, 10000), xmm0);
    __ movdqu(xmm0, Operand(ebx, ecx, times_4, 10000));
    __ movdqu(Operand(ebx, ecx, times_4, 10000), xmm0);

    __ addsd(xmm1, xmm0);
    __ mulsd(xmm1, xmm0);
    __ subsd(xmm1, xmm0);
    __ divsd(xmm1, xmm0);
    __ ucomisd(xmm0, xmm1);
    __ cmpltsd(xmm0, xmm1);

    __ andpd(xmm0, xmm1);
    __ psllq(xmm0, 17);
    __ psllq(xmm0, xmm1);
    __ psrlq(xmm0, 17);
    __ psrlq(xmm0, xmm1);
    __ por(xmm0, xmm1);

    // new instruction introduced by SIMD
    __ cvtdq2ps(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ cvtdq2ps(xmm1, xmm0);
    __ cvtps2dq(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ cvtps2dq(xmm1, xmm0);
    __ paddd(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ paddd(xmm1, xmm0);
    __ psubd(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ psubd(xmm1, xmm0);
    __ pmuludq(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ pmuludq(xmm1, xmm0);
    __ punpackldq(xmm1, Operand(ebx, ecx, times_4, 10000));
    __ punpackldq(xmm1, xmm0);
    {
      __ shufps(xmm1, xmm1, 0x0);
      __ movups(xmm1, Operand(ebx, ecx, times_4, 10000));
      __ movups(Operand(ebx, ecx, times_4, 10000), xmm1);

      __ andps(xmm1, Operand(ebx, ecx, times_4, 10000));
      __ andps(xmm1, xmm0);
      __ xorps(xmm1, Operand(ebx, ecx, times_4, 10000));
      __ xorps(xmm1, xmm0);
      __ orps(xmm1, Operand(ebx, ecx, times_4, 10000));
      __ orps(xmm1, xmm0);

      __ addps(xmm1, Operand(ebx, ecx, times_4, 10000));
      __ addps(xmm1, xmm0);
      __ subps(xmm1, Operand(ebx, ecx, times_4, 10000));
      __ subps(xmm1, xmm0);
      __ mulps(xmm1, Operand(ebx, ecx, times_4, 10000));
      __ mulps(xmm1, xmm0);
      __ divps(xmm1, Operand(ebx, ecx, times_4, 10000));
      __ divps(xmm1, xmm0);
      __ minps(xmm1, Operand(ebx, ecx, times_4, 10000));
      __ minps(xmm1, xmm0);
      __ maxps(xmm1, Operand(ebx, ecx, times_4, 10000));
      __ maxps(xmm1, xmm0);
      __ rcpps(xmm1, Operand(ebx, ecx, times_4, 10000));
      __ rcpps(xmm1, xmm0);
      __ rsqrtps(xmm1, Operand(ebx, ecx, times_4, 10000));
      __ rsqrtps(xmm1, xmm0);
      __ sqrtps(xmm1, Operand(ebx, ecx, times_4, 10000));
      __ sqrtps(xmm1, xmm0);

      __ cmpeqps(xmm1, xmm0);
      __ cmpltps(xmm1, xmm0);
      __ cmpleps(xmm1, xmm0);
      __ cmpneqps(xmm1, xmm0);
      __ cmpnltps(xmm1, xmm0);
      __ cmpnleps(xmm1, xmm0);
    }
  }

  // cmov.
  {
    __ cmov(overflow, eax, Operand(eax, 0));
    __ cmov(no_overflow, eax, Operand(eax, 1));
    __ cmov(below, eax, Operand(eax, 2));
    __ cmov(above_equal, eax, Operand(eax, 3));
    __ cmov(equal, eax, Operand(ebx, 0));
    __ cmov(not_equal, eax, Operand(ebx, 1));
    __ cmov(below_equal, eax, Operand(ebx, 2));
    __ cmov(above, eax, Operand(ebx, 3));
    __ cmov(sign, eax, Operand(ecx, 0));
    __ cmov(not_sign, eax, Operand(ecx, 1));
    __ cmov(parity_even, eax, Operand(ecx, 2));
    __ cmov(parity_odd, eax, Operand(ecx, 3));
    __ cmov(less, eax, Operand(edx, 0));
    __ cmov(greater_equal, eax, Operand(edx, 1));
    __ cmov(less_equal, eax, Operand(edx, 2));
    __ cmov(greater, eax, Operand(edx, 3));
  }

  {
    if (CpuFeatures::IsSupported(SSE4_1)) {
      CpuFeatureScope scope(&assm, SSE4_1);
      __ pextrd(eax, xmm0, 1);
      __ pinsrd(xmm1, eax, 0);
      __ extractps(eax, xmm1, 0);
      __ insertps(xmm1, xmm0, 0);
      __ pmulld(xmm1, Operand(ebx, ecx, times_4, 10000));
      __ pmulld(xmm1, xmm0);
    }
  }

  // AVX instruction
  {
    if (CpuFeatures::IsSupported(AVX)) {
      CpuFeatureScope scope(&assm, AVX);
      __ vaddsd(xmm0, xmm1, xmm2);
      __ vaddsd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vmulsd(xmm0, xmm1, xmm2);
      __ vmulsd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vsubsd(xmm0, xmm1, xmm2);
      __ vsubsd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vdivsd(xmm0, xmm1, xmm2);
      __ vdivsd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vminsd(xmm0, xmm1, xmm2);
      __ vminsd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vmaxsd(xmm0, xmm1, xmm2);
      __ vmaxsd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));

      __ vaddss(xmm0, xmm1, xmm2);
      __ vaddss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vmulss(xmm0, xmm1, xmm2);
      __ vmulss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vsubss(xmm0, xmm1, xmm2);
      __ vsubss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vdivss(xmm0, xmm1, xmm2);
      __ vdivss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vminss(xmm0, xmm1, xmm2);
      __ vminss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vmaxss(xmm0, xmm1, xmm2);
      __ vmaxss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));

      __ vandps(xmm0, xmm1, xmm2);
      __ vandps(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vxorps(xmm0, xmm1, xmm2);
      __ vxorps(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));

      __ vandpd(xmm0, xmm1, xmm2);
      __ vandpd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vxorpd(xmm0, xmm1, xmm2);
      __ vxorpd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
    }
  }

  // FMA3 instruction
  {
    if (CpuFeatures::IsSupported(FMA3)) {
      CpuFeatureScope scope(&assm, FMA3);
      __ vfmadd132sd(xmm0, xmm1, xmm2);
      __ vfmadd132sd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vfmadd213sd(xmm0, xmm1, xmm2);
      __ vfmadd213sd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vfmadd231sd(xmm0, xmm1, xmm2);
      __ vfmadd231sd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));

      __ vfmsub132sd(xmm0, xmm1, xmm2);
      __ vfmsub132sd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vfmsub213sd(xmm0, xmm1, xmm2);
      __ vfmsub213sd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vfmsub231sd(xmm0, xmm1, xmm2);
      __ vfmsub231sd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));

      __ vfnmadd132sd(xmm0, xmm1, xmm2);
      __ vfnmadd132sd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vfnmadd213sd(xmm0, xmm1, xmm2);
      __ vfnmadd213sd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vfnmadd231sd(xmm0, xmm1, xmm2);
      __ vfnmadd231sd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));

      __ vfnmsub132sd(xmm0, xmm1, xmm2);
      __ vfnmsub132sd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vfnmsub213sd(xmm0, xmm1, xmm2);
      __ vfnmsub213sd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vfnmsub231sd(xmm0, xmm1, xmm2);
      __ vfnmsub231sd(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));

      __ vfmadd132ss(xmm0, xmm1, xmm2);
      __ vfmadd132ss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vfmadd213ss(xmm0, xmm1, xmm2);
      __ vfmadd213ss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vfmadd231ss(xmm0, xmm1, xmm2);
      __ vfmadd231ss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));

      __ vfmsub132ss(xmm0, xmm1, xmm2);
      __ vfmsub132ss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vfmsub213ss(xmm0, xmm1, xmm2);
      __ vfmsub213ss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vfmsub231ss(xmm0, xmm1, xmm2);
      __ vfmsub231ss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));

      __ vfnmadd132ss(xmm0, xmm1, xmm2);
      __ vfnmadd132ss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vfnmadd213ss(xmm0, xmm1, xmm2);
      __ vfnmadd213ss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vfnmadd231ss(xmm0, xmm1, xmm2);
      __ vfnmadd231ss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));

      __ vfnmsub132ss(xmm0, xmm1, xmm2);
      __ vfnmsub132ss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vfnmsub213ss(xmm0, xmm1, xmm2);
      __ vfnmsub213ss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
      __ vfnmsub231ss(xmm0, xmm1, xmm2);
      __ vfnmsub231ss(xmm0, xmm1, Operand(ebx, ecx, times_4, 10000));
    }
  }

  // BMI1 instructions
  {
    if (CpuFeatures::IsSupported(BMI1)) {
      CpuFeatureScope scope(&assm, BMI1);
      __ andn(eax, ebx, ecx);
      __ andn(eax, ebx, Operand(ebx, ecx, times_4, 10000));
      __ bextr(eax, ebx, ecx);
      __ bextr(eax, Operand(ebx, ecx, times_4, 10000), ebx);
      __ blsi(eax, ebx);
      __ blsi(eax, Operand(ebx, ecx, times_4, 10000));
      __ blsmsk(eax, ebx);
      __ blsmsk(eax, Operand(ebx, ecx, times_4, 10000));
      __ blsr(eax, ebx);
      __ blsr(eax, Operand(ebx, ecx, times_4, 10000));
      __ tzcnt(eax, ebx);
      __ tzcnt(eax, Operand(ebx, ecx, times_4, 10000));
    }
  }

  // LZCNT instructions
  {
    if (CpuFeatures::IsSupported(LZCNT)) {
      CpuFeatureScope scope(&assm, LZCNT);
      __ lzcnt(eax, ebx);
      __ lzcnt(eax, Operand(ebx, ecx, times_4, 10000));
    }
  }

  // POPCNT instructions
  {
    if (CpuFeatures::IsSupported(POPCNT)) {
      CpuFeatureScope scope(&assm, POPCNT);
      __ popcnt(eax, ebx);
      __ popcnt(eax, Operand(ebx, ecx, times_4, 10000));
    }
  }

  // BMI2 instructions
  {
    if (CpuFeatures::IsSupported(BMI2)) {
      CpuFeatureScope scope(&assm, BMI2);
      __ bzhi(eax, ebx, ecx);
      __ bzhi(eax, Operand(ebx, ecx, times_4, 10000), ebx);
      __ mulx(eax, ebx, ecx);
      __ mulx(eax, ebx, Operand(ebx, ecx, times_4, 10000));
      __ pdep(eax, ebx, ecx);
      __ pdep(eax, ebx, Operand(ebx, ecx, times_4, 10000));
      __ pext(eax, ebx, ecx);
      __ pext(eax, ebx, Operand(ebx, ecx, times_4, 10000));
      __ sarx(eax, ebx, ecx);
      __ sarx(eax, Operand(ebx, ecx, times_4, 10000), ebx);
      __ shlx(eax, ebx, ecx);
      __ shlx(eax, Operand(ebx, ecx, times_4, 10000), ebx);
      __ shrx(eax, ebx, ecx);
      __ shrx(eax, Operand(ebx, ecx, times_4, 10000), ebx);
      __ rorx(eax, ebx, 31);
      __ rorx(eax, Operand(ebx, ecx, times_4, 10000), 31);
    }
  }

  // xchg.
  {
    __ xchg_b(eax, Operand(eax, 8));
    __ xchg_w(eax, Operand(ebx, 8));
    __ xchg(eax, eax);
    __ xchg(eax, ebx);
    __ xchg(ebx, ebx);
    __ xchg(ebx, Operand(esp, 12));
  }

  // cmpxchg.
  {
    __ cmpxchg_b(Operand(esp, 12), eax);
    __ cmpxchg_w(Operand(ebx, ecx, times_4, 10000), eax);
    __ cmpxchg(Operand(ebx, ecx, times_4, 10000), eax);
  }

  // lock prefix.
  {
    __ lock();
    __ cmpxchg(Operand(esp, 12), ebx);

    __ lock();
    __ xchg_w(eax, Operand(ecx, 8));
  }

  // Nop instructions
  for (int i = 0; i < 16; i++) {
    __ Nop(i);
  }

  __ ret(0);

  CodeDesc desc;
  assm.GetCode(&desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  USE(code);
#ifdef OBJECT_PRINT
  OFStream os(stdout);
  code->Print(os);
  byte* begin = code->instruction_start();
  byte* end = begin + code->instruction_size();
  disasm::Disassembler::Disassemble(stdout, begin, end);
#endif
}

#undef __
