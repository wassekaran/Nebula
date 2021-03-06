// ProcessorState.cpp
//
// Copyright 2013 Jesse Haber-Kucharsky
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ProcessorState.hpp"

#include <functional>
#include <sstream>

#include <boost/phoenix/bind.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>

namespace nebula {

Word ProcessorState::read( Register reg ) const noexcept {
    return _registers[registerIndex( reg )];
}

Word ProcessorState::read( Special spec ) const noexcept {
    switch ( spec ) {
    case Special::Pc: return _pc;
    case Special::Sp: return _sp;
    case Special::Ex: return _ex;
    }

    // This shouldn't be necessary, but GCC complains (wrongly) that
    // not all cases are handled in the above.
    return 0;
}

void ProcessorState::write( Register reg, Word value ) noexcept {
    _registers[registerIndex( reg )] = value;
}

void ProcessorState::write( Special spec, Word value ) noexcept {
    switch ( spec ) {
    case Special::Pc: _pc = value; return;
    case Special::Sp: _sp = value; return;
    case Special::Ex: _ex = value; return;
    }
}

static Word fetchNextWord( ProcessorState& proc ) {
    proc.tickClock( 1 );

    auto pc = proc.read( Special::Pc );
    proc.write( Special::Pc, pc + 1 );
    return proc.memory()->read( pc );
}

Word LongAddressingMode::next( ProcessorState& proc ) {
    if ( ! _next ) {
        _next = fetchNextWord( proc );
    }

    return *_next;
}

namespace mode {

Word RegisterIndirectOffset::load( ProcessorState& proc ) {
    return proc.memory()->read( proc.read( reg ) + fetchNextWord( proc ) );
}

void RegisterIndirectOffset::store( ProcessorState& proc, Word value ) {
    proc.memory()->write( proc.read( reg ) + fetchNextWord( proc ), value );
}

void Push::store( ProcessorState& proc, Word value ) {
    auto sp = proc.read( Special::Sp );
    proc.memory()->write( sp - 1, value );
    proc.write( Special::Sp, sp - 1 );
}

Word Pop::load( ProcessorState& proc ) {
    auto sp = proc.read( Special::Sp );
    auto word = proc.memory()->read( sp );
    proc.write( Special::Sp, sp + 1 );

    return word;
}

Word Pick::load( ProcessorState& proc ) {
    auto sp = proc.read( Special::Sp );
    return proc.memory()->read( sp + next( proc ) );
}

void Pick::store( ProcessorState& proc, Word value ) {
    auto sp = proc.read( Special::Sp );
    proc.memory()->write( sp + next( proc ), value );
}

Word Indirect::load( ProcessorState& proc ) {
    return proc.memory()->read( next( proc ) );
}

void Indirect::store( ProcessorState& proc, Word value ) {
    proc.memory()->write( next( proc ), value );
}

Word Direct::load( ProcessorState& proc ) {
    return next( proc );
}

void Direct::store( ProcessorState&, Word ) {
    // Nothing.
}

}

namespace instruction {

void Unary::execute( ProcessorState& proc ) const {
    Word loc;

    switch ( opcode ) {
    case SpecialOpcode::Int:
    case SpecialOpcode::Iag:
    case SpecialOpcode::Ias:
    case SpecialOpcode::Rfi:
    case SpecialOpcode::Iaq:
    case SpecialOpcode::Hwn:
    case SpecialOpcode::Hwq:
    case SpecialOpcode::Hwi:
        // Handled by the parent simulation.
        break;
    case SpecialOpcode::Jsr:
        loc = address->load( proc );
        mode::Push {}.store( proc, proc.read( Special::Pc ) );
        proc.write( Special::Pc, loc );
        break;
    default:
        assert( ! "Unary instruction is unsupported!" );
    }

    proc.tickClock( SPECIAL_OPCODE_CYCLES.at( opcode ) );
}

void Binary::execute( ProcessorState& proc ) const {
    using namespace boost::phoenix::placeholders;

    auto load = [&proc]( std::shared_ptr<AddressingMode> addr ) {
        return addr->load( proc );
    };

    auto store = [&proc]( std::shared_ptr<AddressingMode> addr, Word value ) {
        addr->store( proc, value );
    };

    auto apply = [&] ( std::function<Word( Word, Word )> f ) {
        auto y = load( addressA );
        auto x = load( addressB );

        store( addressB, f( x, y ) );
    };

    auto applyDouble = [&]( std::function<DoubleWord( DoubleWord, DoubleWord )> f,
                            std::function<void( DoubleWord )> action ) {
        auto yd = DoubleWord { load( addressA ) };
        auto xd = DoubleWord { load( addressB ) };
        auto zd = f( xd, yd );

        store( addressB, static_cast<Word>( zd ) );
        action( zd );
    };

    auto applySigned = [&] ( std::function<SignedWord( SignedWord, SignedWord )> f ) {
        auto yi = static_cast<SignedWord>( load( addressA ) );
        auto xi = static_cast<SignedWord>( load( addressB ) );
        auto zi = f( xi, yi );
        store( addressB, static_cast<Word>( zi ) );
    };

    auto skipUnless = [&] ( std::function<bool( Word, Word )> f ) {
        auto y = load( addressA );
        auto x = load( addressB );

        bool skip = ! f( x, y );
        
        if ( skip ) {
            proc.tickClock( 1 );
        }
        
        proc.doSkip = skip;
    };

    auto skipUnlessSigned = [&] ( std::function<bool( SignedWord, SignedWord )> f ) {
        auto y = load( addressA );
        auto x = load( addressB );

        bool skip = ! f( static_cast<SignedWord>( x ),
                         static_cast<SignedWord>( y ) );

        if ( skip ) {
            proc.tickClock( 1 );
        }

        proc.doSkip = skip;
    };

    DoubleWord xd, yd, zd;
    SignedDoubleWord xdi, ydi, zdi;

    switch ( opcode ) {
    case Opcode::Set:
        store( addressB, load( addressA ) );
        break;
    case Opcode::Add:
        applyDouble( arg1 + arg2, [&proc] ( DoubleWord z ) {
                if ( z > 0xffff ) {
                    proc.write( Special::Ex, 1 );
                } else {
                    proc.write( Special::Ex, 0 );
                }
            });
        break;
    case Opcode::Sub:
        applyDouble( arg1 - arg2, [&proc] ( DoubleWord z ) {
                if ( z > 0xffff ) {
                    proc.write( Special::Ex, 0xffff );
                } else {
                    proc.write( Special::Ex, 0 );
                }
            });
        break;
    case Opcode::Mul:
        applyDouble( arg1 * arg2, [&proc] ( DoubleWord z ) {
                proc.write( Special::Ex, (z >> 16) & 0xffff );
            });
        break;
    case Opcode::Mli:
        applySigned( arg1 * arg2 );
        break;
    case Opcode::Div:
        yd = DoubleWord { load( addressA ) };
        xd = DoubleWord { load( addressB ) };
        
        if ( yd == 0 ) {
            zd = 0;
            proc.write( Special::Ex, 0 );
        } else {
            zd = xd / yd;
            proc.write( Special::Ex, ((xd << 16) / yd) & 0xffff );
        }

        store( addressB, static_cast<Word>( zd ) );

        break;
    case Opcode::Dvi:
        applySigned( [] ( SignedWord xi, SignedWord yi ) {
                return yi != 0 ? xi / yi : 0;
            });
        break;
    case Opcode::Mod:
        apply( [] ( Word x, Word y ) {
                return y != 0 ? x % y : 0;
            });
        break;
    case Opcode::Mdi:
        applySigned( [] ( SignedWord xi, SignedWord yi ) {
                return yi != 0 ? xi % yi : 0;
            });
        break;
    case Opcode::And:
        apply( arg1 & arg2 );
        break;
    case Opcode::Bor:
        apply( arg1 | arg2 );
        break;
    case Opcode::Xor:
        apply( arg1 ^ arg2 );
        break;
    case Opcode::Shr:
        yd = DoubleWord { load( addressA ) };
        xd = DoubleWord { load( addressB ) };
        zd = xd >> yd;

        proc.write( Special::Ex, ((xd << 16) >> yd) & 0xffff );
        store( addressB, static_cast<Word>( zd ) );

        break;
    case Opcode::Asr:
        ydi = SignedDoubleWord { load( addressA ) };
        xdi = SignedDoubleWord { load( addressB ) };
        zdi = xdi >> ydi;
        
        proc.write( Special::Ex, ((xdi << 16) >> ydi) & 0xffff );
        store( addressB, static_cast<Word>( zdi ) );

        break;
    case Opcode::Shl:
        yd = DoubleWord { load( addressA ) };
        xd = DoubleWord { load( addressB ) };
        zd = xd << yd;
        
        proc.write( Special::Ex, ((xd << yd) >> 16) & 0xffff );
        store( addressB, static_cast<Word>( zd ) );

        break;
    case Opcode::Ifb:
        skipUnless( (arg1 & arg2) != 0 );
        break;
    case Opcode::Ifc:
        skipUnless( (arg1 & arg2) == 0 );
        break;
    case Opcode::Ife:
        skipUnless( arg1 == arg2 );
        break;
    case Opcode::Ifn:
        skipUnless( arg1 != arg2 );
        break;
    case Opcode::Ifg:
        skipUnless( arg1 > arg2 );
        break;
    case Opcode::Ifa:
        skipUnlessSigned( arg1 > arg2 );
        break;
    case Opcode::Ifl:
        skipUnless( arg1 < arg2 );
        break;
    case Opcode::Ifu:
        skipUnlessSigned( arg1 < arg2 );
        break;
    case Opcode::Adx:
        applyDouble( [&proc] ( DoubleWord x, DoubleWord y ) {
                return x + y + proc.read( Special::Ex );
            }, [&proc] ( DoubleWord z ) {
                proc.write( Special::Ex, z > 0xffff ? 1 : 0 );
            } );
        break;
    case Opcode::Sbx:
        applyDouble( [&proc] ( DoubleWord x, DoubleWord y ) {
                return x - y + proc.read( Special::Ex );
            }, [&proc] ( DoubleWord z ) {
                proc.write( Special::Ex, z > 0xffff ? 1 : 0 );
            } );
        break;
    case Opcode::Sti:
        store( addressB, load( addressA ) );
        proc.write( Register::I, proc.read( Register::I ) + 1 );
        proc.write( Register::J, proc.read( Register::J ) + 1 );
        break;
    case Opcode::Std:
        store( addressB, load( addressA ) );
        proc.write( Register::I, proc.read( Register::I ) - 1 );
        proc.write( Register::J, proc.read( Register::J ) - 1 );
        break;
    }

    // Tick the clock the appropriate number of times.
    proc.tickClock( OPCODE_CYCLES.at( opcode ) );
}

}

template <>
optional<Register> decode( const Word &w ) {
    switch ( w ) {
    case 0: return Register::A;
    case 1: return Register::B;
    case 2: return Register::C;
    case 3: return Register::X;
    case 4: return Register::Y;
    case 5: return Register::Z;
    case 6: return Register::I;
    case 7: return Register::J;
    default: return {};
    }
}

template <>
optional<Opcode> decode( const Word& w ) {
    switch ( w ) {
    case 0x01: return Opcode::Set;
    case 0x02: return Opcode::Add;
    case 0x03: return Opcode::Sub;
    case 0x04: return Opcode::Mul;
    case 0x05: return Opcode::Mli;
    case 0x06: return Opcode::Div;
    case 0x07: return Opcode::Dvi;
    case 0x08: return Opcode::Mod;
    case 0x09: return Opcode::Mdi;
    case 0x0a: return Opcode::And;
    case 0x0b: return Opcode::Bor;
    case 0x0c: return Opcode::Xor;
    case 0x0d: return Opcode::Shr;
    case 0x0e: return Opcode::Asr;
    case 0x0f: return Opcode::Shl;
    case 0x10: return Opcode::Ifb;
    case 0x11: return Opcode::Ifc;
    case 0x12: return Opcode::Ife;
    case 0x13: return Opcode::Ifn;
    case 0x14: return Opcode::Ifg;
    case 0x15: return Opcode::Ifa;
    case 0x16: return Opcode::Ifl;
    case 0x17: return Opcode::Ifu;
    case 0x1a: return Opcode::Adx;
    case 0x1b: return Opcode::Sbx;
    case 0x1e: return Opcode::Sti;
    case 0x1f: return Opcode::Std;
    default: return {};
    }
}

template <>
optional<SpecialOpcode> decode( const Word& w ) {
    switch ( w ) {
    case 0x01: return SpecialOpcode::Jsr;
    case 0x08: return SpecialOpcode::Int;
    case 0x09: return SpecialOpcode::Iag;
    case 0x0a: return SpecialOpcode::Ias;
    case 0x0b: return SpecialOpcode::Rfi;
    case 0x10: return SpecialOpcode::Hwn;
    case 0x11: return SpecialOpcode::Hwq;
    case 0x12: return SpecialOpcode::Hwi;
    default: return {};
    }
}

static std::shared_ptr<AddressingMode>
decodeRegisterDirect( Word w ) {
    auto reg = decode<Register>( w );

    if ( reg ) {
        return std::make_shared<mode::RegisterDirect>( *reg );
    } else {
        return nullptr;
    }
}

static std::shared_ptr<AddressingMode>
decodeRegisterIndirect( Word w ) {
    auto reg = decode<Register>( w - 0x8 );

    if ( reg ) {
        return std::make_shared<mode::RegisterIndirect>( *reg );
    } else {
        return nullptr;
    }
}

static std::shared_ptr<AddressingMode>
decodeRegisterIndirectOffset( Word w ) {
    auto reg = decode<Register>( w - 0x10 );

    if ( reg ) {
        return std::make_shared<mode::RegisterIndirectOffset>( *reg );
    } else {
        return nullptr;
    }
}

static
std::shared_ptr<AddressingMode>
decodeFastDirect( Word w ) {
    Word value;

    if ( w >= 0x20 && w <= 0x3f ) {
        if ( w == 0x20 ) {
            value = 0xffff;
        } else {
            value = w - 0x21;
        }

        return std::make_shared<mode::FastDirect>( value );
    }

    return nullptr;
}

template <typename T>
static std::function<std::shared_ptr<T>( Word )>
decoderByValue( Word value ) {
    return [value]( Word w ) -> std::shared_ptr<T> {
        if ( w == value ) {
            return std::make_shared<T>();
        } else {
            return nullptr;
        }
    };
}

std::shared_ptr<AddressingMode>
decodeAddress( AddressContext context, Word word ) {
    std::shared_ptr<AddressingMode> addr = nullptr;

    auto decodePush = decoderByValue<mode::Push>( 0x18 );
    auto decodePop = decoderByValue<mode::Pop>( 0x18 );
    auto decodePeek = decoderByValue<mode::Peek>( 0x19 );
    auto decodePick = decoderByValue<mode::Pick>( 0x1a );
    auto decodeSp = decoderByValue<mode::Sp>( 0x1b );
    auto decodePc = decoderByValue<mode::Pc>( 0x1c );
    auto decodeEx = decoderByValue<mode::Ex>( 0x1d );
    auto decodeIndirect = decoderByValue<mode::Indirect>( 0x1e );
    auto decodeDirect = decoderByValue<mode::Direct>( 0x1f );

#define TRY( f ) addr = f( word ); if ( addr ) return addr;

    if ( context == AddressContext::A ) {
        TRY( decodePop );
        TRY( decodeFastDirect );
    }

    if ( context == AddressContext::B ) {
        TRY( decodePush );
    }

    TRY( decodeRegisterDirect );
    TRY( decodeRegisterIndirect );
    TRY( decodeRegisterIndirectOffset );
    TRY( decodePush );
    TRY( decodePeek );
    TRY( decodePick );
    TRY( decodeSp );
    TRY( decodePc );
    TRY( decodeEx );
    TRY( decodeIndirect );
    TRY( decodeDirect );
    
#undef TRY

    return {};
}

static std::shared_ptr<Instruction>
decodeBinaryInstruction( Word word ) {
    auto opcode = decode<Opcode>( word & 0x1f );
    if ( ! opcode ) return nullptr;

    auto addrA = decodeAddress( AddressContext::A, (word & 0xfc00) >> 10 );
    if ( ! addrA ) return nullptr;

    auto addrB = decodeAddress( AddressContext::B, (word & 0x3e0) >> 5 );
    if ( ! addrB ) return nullptr;

    return std::make_shared<instruction::Binary>( *opcode, addrB, addrA );
}

static std::shared_ptr<Instruction>
decodeUnaryInstruction( Word word ) {
    auto opcode = decode<SpecialOpcode>( (word & 0x3e0) >> 5 );
    if ( ! opcode ) return nullptr;

    auto addr = decodeAddress( AddressContext::A, (word & 0xfc00) >> 10 );
    if ( ! addr ) return nullptr;

    return std::make_shared<instruction::Unary>( *opcode, addr );
}

std::shared_ptr<Instruction> decodeInstruction( Word word ) {
    auto bins = decodeBinaryInstruction( word );
    if ( bins ) return bins;

    auto uins = decodeUnaryInstruction( word );
    if ( uins ) return uins;

    return nullptr;
}

static std::shared_ptr<Instruction> fetchNextInstruction( ProcessorState& proc ) {
    auto word = fetchNextWord( proc );
    auto ins = decodeInstruction( word );

    if ( ins ) {
        return ins;
    } else {
        throw error::MalformedInstruction { word };
    }
}

void ProcessorState::executeNext() {
    // !!!
    // dumpToLog( *this );

    auto ins = fetchNextInstruction( *this );

    if ( doSkip ) {
        advance( *this, ins->size() );

        // If it's a conditional instruction, then continue skipping at the cost of
        // of one cycle.
        if ( ins->isConditional() ) {
            tickClock( 1 );
        } else {
            doSkip = false;
        }
    } else {
        ins->execute( *this );
        _lastInstruction = ins;
    }
}

void advance( ProcessorState& proc, int numWords ) {
    auto pc = proc.read( Special::Pc );
    proc.write( Special::Pc, pc + numWords );
}

void dumpToLog( ProcessorState& proc ) {
    LOG( PSTATE, info ) << "\n";
    LOG( PSTATE, info ) << format( "PC    : 0x%04x" ) % proc.read( Special::Pc );
    LOG( PSTATE, info ) << format( "SP    : 0x%04x" ) % proc.read( Special::Sp );
    LOG( PSTATE, info ) << format( "EX    : 0x%04x" ) % proc.read( Special::Ex );
    LOG( PSTATE, info ) << format( "A     : 0x%04x" ) % proc.read( Register::A );
    LOG( PSTATE, info ) << format( "B     : 0x%04x" ) % proc.read( Register::B );
    LOG( PSTATE, info ) << format( "C     : 0x%04x" ) % proc.read( Register::C );
    LOG( PSTATE, info ) << format( "X     : 0x%04x" ) % proc.read( Register::X );
    LOG( PSTATE, info ) << format( "Y     : 0x%04x" ) % proc.read( Register::Y );
    LOG( PSTATE, info ) << format( "Z     : 0x%04x" ) % proc.read( Register::Z );
    LOG( PSTATE, info ) << format( "I     : 0x%04x" ) % proc.read( Register::I );
    LOG( PSTATE, info ) << format( "J     : 0x%04x" ) % proc.read( Register::J );
    LOG( PSTATE, info ) << format( "skip  : %s" ) % proc.doSkip;

    std::stringstream stackContents;
    const int STACK_SIZE = processor::STACK_BEGIN - proc.read( Special::Sp );
    const int DISPLAYED_STACK_SIZE = std::min( 5, STACK_SIZE );

    auto stackCell = [&stackContents, &proc] ( int index ) {
        return format( "0x%04x" ) % proc.memory()->read( proc.read( Special::Sp ) + index );
        
    };

    for ( int i = 0; i < DISPLAYED_STACK_SIZE - 1; ++i ) {
        stackContents << stackCell( i );
        stackContents << ",";
    }

    if ( DISPLAYED_STACK_SIZE > 0 ) {
        stackContents << stackCell( DISPLAYED_STACK_SIZE - 1 );
    }

    if ( DISPLAYED_STACK_SIZE < STACK_SIZE ) {
        stackContents << ", ...";
    }

    LOG( PSTATE, info ) << "stack : " << stackContents.str();
    LOG( PSTATE, info ) << "\n";
}

}
