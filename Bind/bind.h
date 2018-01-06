#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>
using std::cout;
using std::endl;

template <typename T, T ... values>
struct integer_sequence
{};

template <typename T, typename Seq1, T arg>
struct append;

template <typename T, T ... values, T arg>
struct append<T, integer_sequence<T, values...>, arg>
{
    typedef integer_sequence<T, values..., arg> type;
};

template <typename T, typename Seq1, typename Seq2>
struct append_sequences;

template <int ... seq1, int ...seq2>
struct append_sequences <int, integer_sequence<int, seq1...>, integer_sequence<int, seq2...> >
{
    typedef integer_sequence<int, seq1..., seq2...> type;
};

template <typename T, T N, typename Q = void>
struct make_integer_sequence
{
    typedef typename append<T, typename make_integer_sequence<T, N - 1>::type, N - 1>::type type;
};

template <typename T, T N>
struct make_integer_sequence<T, N, typename std::enable_if<N == 0>::type>
{
    typedef integer_sequence<T> type;
};

template <int N>
struct placeholder
{};

template <typename F, typename ... As>
struct bind_t;

constexpr placeholder<1> _1;
constexpr placeholder<2> _2;
constexpr placeholder<3> _3;

template<typename T>
struct ph_cleaner
{
    typedef T type;
};

template <int N>
struct ph_cleaner<placeholder<N>&>
{
    typedef placeholder<N> type;
};

template <int N>
struct ph_cleaner<const placeholder<N>&>
{
    typedef placeholder<N> type;
};

template <int N>
struct ph_cleaner<placeholder<N>&&>
{
    typedef placeholder<N> type;
};

template<typename T>
using ph_cleaner_t = typename ph_cleaner<T>::type;

template<int X, typename Seq>
struct check_unique;

template<int X, int N, int ... seq>
struct check_unique<X, integer_sequence<int, N, seq...>>
{
    static constexpr int cnt = static_cast<int>(X == N) + check_unique<X, integer_sequence<int, seq...>>::cnt;
};

template<int X>
struct check_unique<X, integer_sequence<int>>
{
    static constexpr int cnt = 0;
};

template<int X, typename Seq>
constexpr bool unique = check_unique<X, Seq>::cnt <= 1;

template <typename T>
struct one_arg_to_ph_num
{
    typedef integer_sequence<int, 0> value;
};

template <int N>
struct one_arg_to_ph_num<placeholder<N>>
{
    typedef integer_sequence<int, N> value;
};

template<typename ... Seq>
struct args_to_ph_num;

template<typename T, typename ...Seq>
struct args_to_ph_num<T, Seq...>
{
    typedef
    typename append_sequences<
    int,
    typename one_arg_to_ph_num<T>::value,
    typename args_to_ph_num<Seq...>::value>
    ::type
    value;
};

template<>
struct args_to_ph_num<>
{
    typedef make_integer_sequence<int, 0>::type value;
};

template<typename ... T>
using args_to_ph_num_t = typename args_to_ph_num<std::decay_t<T>...>::value;

template<typename T, bool f>
struct get_reference
{
    typedef T& type;
};

template<typename T>
struct get_reference<T, 1>
{
    typedef T&& type;
};

template<typename T, bool f>
using get_reference_t = typename get_reference<T, f>::type;


template <typename A>
struct G
{
    template<typename RA>
    G(RA&& aa) : a(std::forward<RA>(aa)) {}

    template <typename ... Bs>
    A operator()(Bs&&...)
    {
        return static_cast<A>(a);
    }
private:
    std::remove_reference_t<A> a;
};

template <>
struct G<placeholder<1> >
{
    G(placeholder<1>)
    {}

    template <typename B1, typename ... Bs>
    decltype(auto) operator()(B1&& b1, Bs&& ...)
    {
        return std::forward<B1>(b1);
    }
};

template <int N>
struct G<placeholder<N> >
{
    G(placeholder<N>)
    {}

    template <typename B, typename ... Bs>
    decltype(auto) operator()(B&&, Bs&& ... bs)
    {
        G<placeholder<N - 1>> next((placeholder<N - 1>()));
        return next(std::forward<Bs>(bs)...);
    }
};

template <typename F, typename ... As>
struct G<bind_t<F, As...> >
{
    G(bind_t<F, As...>&& b) :
        fun(std::move(b))
    {}

    G(const bind_t<F, As...>& b) :
        fun(b)
    {}

    template <typename ... Bs>
    decltype(auto) operator()(Bs&& ... bs)
    {
        return fun(std::forward<Bs>(bs)...);
    }

    bind_t<F, As...> fun;
};

template<typename T>
struct internal_bind_cleaner
{
    typedef T value;
};

template<typename F, typename ... As>
struct internal_bind_cleaner<bind_t<F, As...>&>
{
    typedef bind_t<F, As...> value;
};

template<typename F, typename ... As>
struct internal_bind_cleaner<const bind_t<F, As...>&>
{
    typedef bind_t<F, As...> value;
};

template<typename F, typename ... As>
struct internal_bind_cleaner<bind_t<F, As...>&&>
{
    typedef bind_t<F, As...> value;
};

template<typename T>
using internal_bind_cleaner_t = typename internal_bind_cleaner<T>::value;

template <typename F, typename ... As>
struct bind_t
{
    template<typename RF, typename ... RAs>
    bind_t(RF&& f, RAs&& ... as)
        : f(std::forward<RF>(f))
        , gs(std::forward<RAs>(as)...)
    {}

    template <typename ... Bs>
    decltype(auto) operator()(Bs&& ... bs)
    {
        return call(typename make_integer_sequence<int, sizeof...(As)>::type(),
                    typename make_integer_sequence<int, sizeof...(Bs)>::type(),
                    args_to_ph_num_t<As...>(),
                    std::forward<Bs>(bs)...);
    }
private:
    template <int ... as_inds, int ... bs_inds, int ... ph_nums, typename ... Bs>
    decltype(auto) call(integer_sequence<int, as_inds...>,
                        integer_sequence<int, bs_inds...>,
                        integer_sequence<int, ph_nums...> ph_seq,
                        Bs&& ... bs)
    {
        return f(std::get<as_inds>(gs)(
                     static_cast<get_reference_t<Bs, unique<bs_inds + 1, decltype(ph_seq)>>>(bs)...)...);
    }
private:
    F f;
    std::tuple<G<internal_bind_cleaner_t<As>>...> gs;
};

template <typename F, typename ... As>
decltype(auto) bind(F&& f, As&& ... as)
{
    return bind_t<std::decay_t<F>, ph_cleaner_t<std::decay_t<As>&>...>(std::forward<F>(f), std::forward<As>(as)...);
}

template <typename F, typename ... As>
decltype(auto) call_once_bind(F&& f, As&& ... as)
{
    return bind_t<std::decay_t<F>, ph_cleaner_t<std::decay_t<As>&&>...>(std::forward<F>(f), std::forward<As>(as)...);
}
