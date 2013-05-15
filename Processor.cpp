#include "Processor.hpp"

#include <boost/variant.hpp>

using boost::variant;
using boost::static_visitor;
using boost::apply_visitor;

Word Processor::read( Register reg ) const {
    return _registers[registerIndex( reg )];
}

Word Processor::read( Special spec ) const {
    switch ( spec ) {
    case Special::Pc: return _pc;
    case Special::Sp: return _sp;
    case Special::Ex: return _ex;
    }
}

Word Processor::read( Word loc) const {
    if ( loc >= _memory.size() ) {
        throw error::InvalidMemoryLocation { MemoryOperation::Read, loc };
    }

    return _memory[loc];
}

void Processor::write( Register reg, Word value ) {
    _registers[registerIndex( reg )] = value;
}

void Processor::write( Special spec, Word value ) {
    switch ( spec ) {
    case Special::Pc: _pc = value; return;
    case Special::Sp: _sp = value; return;
    case Special::Ex: _ex = value; return;
    }
}

void Processor::write( Word loc, Word value ) {
    if ( loc >= _memory.size() ) {
        throw error::InvalidMemoryLocation { MemoryOperation::Write, loc };
    }

    _memory[loc] = value;
}

static Word fetchNextWord( Processor& proc ) {
    return address::read( proc, address::direct() );
}

namespace address {

Address registerDirect( const Register& reg ) {
    Address addr;
    addr.type = AddressType::RegisterDirect;
    addr.reg = reg;
    return addr;
}

Address registerIndirect( const Register& reg ) {
    Address addr;
    addr.type = AddressType::RegisterIndirect;
    addr.reg = reg;
    return addr;
}

Address registerIndirectOffset( const Register& reg ) {
    Address addr;
    addr.type = AddressType::RegisterIndirectOffset;
    addr.reg = reg;
    return addr;
}

Address push() {
    Address addr;
    addr.type = AddressType::Push;
    return addr;
}

Address pop() {
    Address addr;
    addr.type = AddressType::Pop;
    return addr;
}

Address peek() {
    Address addr;
    addr.type = AddressType::Peek;
    return addr;
}

Address pick() {
    Address addr;
    addr.type = AddressType::Pick;
    return addr;
}

Address sp() {
    Address addr;
    addr.type = AddressType::Sp;
    return addr;
}

Address pc() {
    Address addr;
    addr.type = AddressType::Pc;
    return addr;
}

Address ex() {
    Address addr;
    addr.type = AddressType::Ex;
    return addr;
}

Address indirect() {
    Address addr;
    addr.type = AddressType::Indirect;
    return addr;
}

Address direct() {
    Address addr;
    addr.type = AddressType::Direct;
    return addr;
}

Address fastDirect( Word word ) {
    Address addr;
    addr.type = AddressType::FastDirect;
    addr.word = word;
    return addr;
}

bool isLiteral( const Address& addr ) {
    switch ( addr.type ) {
    case AddressType::FastDirect:
    case AddressType::Direct:
        return true;
    default:
        return false;
    }
}

using Location = variant<Word, Register, Special>;

static Location location( Processor& proc, const Address& addr ) {
    Word sp = proc.read( Special::Sp );
    Word pc = proc.read( Special::Pc );
    Word ex = proc.read( Special::Ex );

    switch ( addr.type ) {
    case AddressType::RegisterDirect: return addr.reg;
    case AddressType::RegisterIndirect: return proc.read( addr.reg );
    case AddressType::RegisterIndirectOffset:
        return fetchNextWord( proc ) + proc.read( addr.reg );
    case AddressType::Push:
        if ( sp == 0 ) {
            throw error::StackOverflow {};
        }

        proc.write( Special::Sp, sp - 1);
        return sp - 1;
    case AddressType::Pop:
        if ( sp >= proc.memorySize() - 1 ) {
            throw error::StackUnderflow {};
        }

        proc.write( Special::Sp, sp + 1 );
        return sp;
    case AddressType::Peek: return sp;
    case AddressType::Pick: return sp + fetchNextWord( proc );
    case AddressType::Sp: return sp;
    case AddressType::Pc: return pc;
    case AddressType::Ex: return ex;
    case AddressType::Indirect: return fetchNextWord( proc );
    case AddressType::Direct:
        proc.tickClock( 1 );
        proc.write( Special::Pc, pc + 1 );
        return pc;
    case AddressType::FastDirect: return addr.word;
    }
}

struct ReadVisitor : public static_visitor<Word> {
    Processor* proc = nullptr;

    ReadVisitor( Processor* proc ) :
        proc { proc } {}

    template <typename LocationType>
    Word operator()( const LocationType& loc ) const {
        return proc->read( loc );
    }
};

Word read( Processor& proc, const Address& addr ) {
    if ( addr.type == AddressType::FastDirect ) {
        return addr.word;
    } else {
        auto loc = location( proc, addr );
        return apply_visitor( ReadVisitor { &proc }, loc );
    }
}

struct WriteVisitor : public static_visitor<void> {
    Processor* proc = nullptr;
    Word value;

    WriteVisitor( Processor* proc, const Word& value ) :
        proc { proc },
        value { value } {}

    template <typename LocationType>
    void operator()( const LocationType& loc ) const {
        proc->write( loc, value );
    }
};

void write( Processor& proc, const Address& addr, Word value ) {
    if ( isLiteral( addr ) ) {
        // Nothing.
    } else {
        auto loc = location( proc, addr );
        apply_visitor( WriteVisitor { &proc, value }, loc );
    }
}

}


