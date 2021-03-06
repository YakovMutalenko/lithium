#pragma once

#include <mysql.h>

#include <li/callable_traits/callable_traits.hh>
#include <li/metamap/metamap.hh>
#include <li/sql/sql_common.hh>
#include <li/sql/symbols.hh>

#include <li/sql/internal/mysql_bind.hh>
#include <li/sql/internal/mysql_statement_data.hh>
#include <li/sql/mysql_async_wrapper.hh>

namespace li {

/**
 * @brief The prepared statement result interface.
 *
 * @tparam B
 */
template <typename B> struct mysql_statement_result {

  /**
   * @return the number of rows affected by the request.
   */
  long long int affected_rows();

  /**
   * @brief access to the result of a single valued request (ex: select 1+2) and return the value.
   * Throw an exception if the request returned nothing.
   *
   * @return T the result value.
   */
  template <typename T> T read();

  /**
   * @brief acces to the result of single valued request (ex: select 1+2) and return a optinal value
   * wrapped with std::optional. the optional is empty if the request returned nothing.
   * @return std::optional<T> the result optional value.
   */
  template <typename T> std::optional<T> read_optional();

  /**
   * @brief Like read_optional but inplace.
   *
   * @tparam T
   * @param o the ouput optional value.
   */
  template <typename T> void read(std::optional<T>& o);

  /**
   * @brief Call a function on each result row.
   * The function must take as argument all the select fields of the request.
   *
   * @example connection.prepare("Select id,post from post_items;")().map(
   *        [&](std::string id, std::string post) {
   *             std::cout << id << post << std::endl; });
   *
   * @param f the function.
   */
  template <typename F> void map(F f);

  /**
   * @brief Return the last id generated by a insert comment.
   *
   * @return the last inserted id.
   */
  long long int last_insert_id();

  /**
   * @return Return true if the statement returned no row.
   */
  bool empty();

  // Internal methods.
  void wait();
  template <typename... A>
  void finalize_fetch(MYSQL_BIND* bind, unsigned long* real_lengths, metamap<A...>& o);
  template <typename... A>
  void finalize_fetch(MYSQL_BIND* bind, unsigned long* real_lengths, std::tuple<A...>& o);
  template <typename A> void finalize_fetch(MYSQL_BIND* bind, unsigned long* real_lengths, A& o);
  template <typename T> void fetch_column(MYSQL_BIND*, unsigned long, T&, int);

  void fetch_column(MYSQL_BIND* b, unsigned long real_length, std::string& v, int i);
  template <unsigned SIZE>
  void fetch_column(MYSQL_BIND& b, unsigned long real_length, sql_varchar<SIZE>& v, int i);
  template <typename T> int fetch(T&& o);

  B& mysql_wrapper_;
  mysql_statement_data& data_;
  std::shared_ptr<int> connection_status_;
};


} // namespace li

#include <li/sql/mysql_statement_result.hpp>
