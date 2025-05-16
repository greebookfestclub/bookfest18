= Git の commit log はソフトウェアアーキテクチャを考えるヒントになりうるか -GitQL と jq を用いて commit log を分析する-

== はじめに
こんにちは。 REALITY 株式会社で主にサーバーサイドのエンジニアをしている kunado です。今回は、 Git のコミットログの分析について考えたこと、実際に試したことについてご紹介します。

== 背景
=== 単一責任原則(SRP: Single Responsibility Principle)
ソフトウェア開発における重要な設計原則の一つに、「単一責任の原則（Single Responsibility Principle 以下、SRP）」があります。 SRP は、 Robert C. Martin 氏(通称 Uncle Bob)によって提唱されており、同氏のブログ記事「The Single Responsibility Principle@<fn>{srp_bob}」で詳細に解説されています。この記事における SRP の核心は、「モジュールは変更するための理由を1つだけ持つべきである」という考え方です。

//footnote[srp_bob][https://blog.cleancoder.com/uncle-bob/2014/05/08/SingleReponsibilityPrinciple.html]

ここで言う「責任」とは、単にプログラム内の機能やタスクを指すのではなく、むしろ、プログラムが特定の「利害関係者」や「アクター」に対して負うべき変更の軸となるものを指します。 Uncle Bob 氏は記事の中で、ある Employee クラスを例に挙げ、このクラスが給与計算、データベースへの保存、勤務時間の報告といった複数のメソッドを持つ場合、これらはそれぞれ異なる部門（例えば経理部門、人事部門、運用部門）の関心事に対応する可能性があると指摘します。もし、これらの異なる関心事に関連する変更が同じモジュール（この場合は Employee クラス）に集中すると、ある部門のための変更が、意図せず他の部門の機能に影響を与えてしまうリスクが高まります。

SRP の目的は、このように異なる理由で変更されうる要素を分離することにあります。つまり、あるモジュールを変更する理由は、理想的にはただ一つであるべきです。これにより、各モジュールは特定のビジネス機能や関心事に特化し、それぞれの変更が他に予期せぬ影響を及ぼすことを防ぎます。結果として、ソフトウェア全体の凝集度が高まり、結合度が適切に管理され、保守性や理解しやすさが向上するのです。

さて、このような視点に立つと、あるファイルが頻繁に変更されていることがわかった場合、それはそのファイルが単一の責務ではなく、複数の責務を負っている可能性があることのシグナルとして捉えることができるはずです。

Git でコードベースをバージョン管理していれば、どのファイルがどれだけ変更されているかという情報は、コミットログに記録されています。であるならば、コミットログを分析することを通じて、複数の責務を負っているファイルが存在するならば、その候補を見つけ出すことができるはずです。

今回は、このような仮説に基づいて、Git のコミットログから変更頻度の高いファイルを見つけ出す方法を検討しました。

== アプローチ方法
ここからは、実際にどのようにコミットログを分析するか、アプローチについて説明していきます。本記事では、GitQL を用いてコミットログの分析を行います。

=== GitQL について
GQL(Git Query Language)@<fn>{gql} は、Gitリポジトリに対して、 SQL ライクなクエリを実行できる問い合わせ言語とその処理系です。 AmrDeveloper 氏によって開発され、GitHub上で公開されています。

//footnote[gql][https://github.com/amrdeveloper/GQL]

GQL（Git Query Language）は、Gitリポジトリを直接読み込み、 SQL ライクな構文を使用して Git リポジトリのデータに対してクエリを実行できるツールです。このツールを使うことで、 Git のコミット履歴、ブランチ、タグ、ファイル差分などの情報を、あたかもデータベースのテーブルであるかのように扱うことができます。
GQL の大きな利点は、.gitファイルを別のデータベース形式に変換したり、データをエクスポートしたりすることなく、その場でクエリを実行できることです。これにより、 Git リポジトリの現状を素早く、柔軟に分析することが可能になります。
commits, diffs, diffs_changes といったテーブルに対して、標準的な SQL のステートメントである SELECT を用いたデータの抽出や、 WHERE によるフィルタリング、 GROUP BY による集計、 HAVING による集計結果のフィルタリングなどが利用可能です。

== 分析の手順と実際のクエリ
ここからは、実際のクエリの例を交えながらコミットログの分析を行う手順を紹介します。今回は分析の手順を示すために、分析の対象として Ruby on Rails のリポジトリ(https://github.com/rails/rails)を用います。

手始めに、直近 2 週間以内のコミットを取得してみましょう。以下のようなクエリで取得できます(このクエリでは、 マージコミットを取り除くために parents_count != 2 の条件を含めています)。

//emlist{
SELECT
  commit_id,
  title,
  parents_count,
  datetime,
  CURRENT_TIMESTAMP()
FROM
  commits
WHERE
  DATE(datetime) >= DATE(CURRENT_TIMESTAMP()) - 14 AND
  parents_count != 2
ORDER BY
  datetime DESC
LIMIT 10000;
//}

このクエリを @<tt>{commits.gql} として保存します。このクエリを実行してみましょう。

//cmd{
$ gitql -r . -q "$(cat commits.gql)" --output json | jq .
[
  {
    "commit_id": "0109d9d68a6b0ae3397bb69c5e2f6283c0d138e1",
    "datetime": "2025-05-15 17:47:09.000",
    "parents_count": "1",
    "title": "[ci skip][docs] Keep Product model associations consistent"
  },
...
//}

このように、コミットを取得することができました。

では次は、この取得したコミットのデータを加工して、diffs_changes テーブルから変更のあったファイルパスを取得してみます。@<fn>{cannot_join}
まず、先ほどのクエリを用いて、 jq コマンドを使って加工することで commit_id のみを取り出してみます。

//footnote[cannot_join][今回は commits テーブルと diffs_changes テーブルとを2回のクエリにわけて取得しています。 GitQLは SQL 文における JOIN 句もサポートしているとドキュメントにも記載されています。しかし実際に JOIN を用いてクエリを書いて実行してみると、期待とは異なる出力結果が得られることがあったため、今回は別々のクエリで取得することにしています。]

//cmd{
$ gitql -r . -q  "$(cat commits.gql)" --output json | jq -r .[].commit_id
0109d9d68a6b0ae3397bb69c5e2f6283c0d138e1
d4f8806344bd554a4f0703620af7429667c0aa6d
88ef03632490f9dcecf60ca8219d662fd8e42ce4
...
//}

これを次のようなコマンドを使ってもう少し加工します。

//cmd{
$ gitql -r . -q  "$(cat commits.gql)" --output json | jq -r .[].commit_id | sed 's/.*/ commit_id = "&"/; $!s/$/ OR /'
 commit_id = "0109d9d68a6b0ae3397bb69c5e2f6283c0d138e1" OR
 commit_id = "d4f8806344bd554a4f0703620af7429667c0aa6d" OR
 commit_id = "88ef03632490f9dcecf60ca8219d662fd8e42ce4" OR
 ...
 commit_id = "9253e6e06777d526bbb6adcdd1af836f4efc8225"
//}

この出力結果を用いて、 @<tt>{diffs_changes.gql} というクエリを作成します。@<fn>{cannot_use_in}

//footnote[cannot_use_in][こちらも通常の SQL 文であれば @<tt>{commit_id IN (...)} などと書けるところですが、それではうまくクエリを実行できなかったため、泥臭く OR 演算子による条件の結合を用いて WHERE 句を書いています。]

//emlist{
SELECT
  commit_id,
  path,
FROM
  diffs_changes
WHERE
  commit_id = "0109d9d68a6b0ae3397bb69c5e2f6283c0d138e1" OR
  commit_id = "d4f8806344bd554a4f0703620af7429667c0aa6d" OR
  commit_id = "88ef03632490f9dcecf60ca8219d662fd8e42ce4" OR
  ...
  commit_id = "9253e6e06777d526bbb6adcdd1af836f4efc8225"
LIMIT 10000;
//}

以下のように実行します。

//cmd{
$ gitql -r . -q  "$(cat diffs_changes.gql)" --output json  | jq -r .
[
  {
    "commit_id": "9ee0d93fc378163a44ed3085d2641b65408508b8",
    "path": "guides"
  },
  ...
//}

commit_id とそのコミットに含まれる path の対応関係が JSON の配列として出力されました。
今度はこれを用いて、ファイルパスごとの出現回数を調べてみましょう。 jq コマンドを使って整形しますが、今回のクエリはやや複雑なので、以下のクエリを @<tt>{filter_files.jq} というファイルとして保存しておきます。このクエリでは、 path の値がファイルではなくディレクトリを示しているエントリーを取り除いています。

//emlist{
. as $all_items |
($all_items | map(.path) | unique) as $unique_paths |
map(
  select(
    .path as $current_path |
    ( ($unique_paths | map(startswith($current_path + "/")) | any) | not )
  )
)
//}

それでは、以下のように実行しましょう。

//cmd{
$ gitql -r . -q  "$(cat diffs_changes.gql)" --output json  | jq -r "$(cat filter_files.jq)" | jq 'group_by(.path) | map({path: .[0].path, count: length}) | sort_by(-.count)'
[
  {
    "path": "Gemfile.lock",
    "count": 7
  },
  {
    "path": "activerecord/lib/active_record/relation/query_methods.rb",
    "count": 3
  },
  {
    "path": "actionpack/actionpack.gemspec",
    "count": 2
  },
  ...
//}

path の名前とその出現頻度を取得することができました。
今回取得したコミットの中では @<tt>{Gemfile.lock} が最も更新されていたようです。これに関してはさもありなんな結果ではありますが、ロジックが含まれるファイルではなく、今回の趣旨には沿わないのでこのファイルは除外して考えることにしましょう。
その次には、 @<tt>{query_methods.rb} というファイルが最も頻繁に更新されていたようです。以下のコマンドの結果から、ファイルの変更の総数は80なので、そのうちの3%程度がこのファイルに対する変更であることがわかります。
//cmd{
$ gitql -r . -q  "$(cat diffs_changes.gql)" --output json  | jq -r "$(cat filter_files.jq)" | jq 'length'
80
//}

== 最後に
さて、ここまで Git のコミットログを用いて、単一責任原則の観点からソフトウェアアーキテクチャについて分析・考察する方法について検討しました。一連の分析の流れ自体は実際に実行可能なものであることがわかりました。
GitQL というツールは、やや痒いところに手が届かないような部分はありますが、 Git を操作するライブラリを用いたコードをスクラッチで記述するよりも、 SQL についての多少の知識さえあれば直感的に扱えるツールであるという印象を持ちました。
今回はファイルの変更頻度について着目しましたが、 Git のコミットログにはそれ以外にもたくさんの情報が含まれており、別の観点では「コミットログから誰がどの領域についての知識を多く持っているか」という視点で分析するなど、業務で普段メンテナンスしているコードベースに対して分析をかければ、活用できる方法はたくさんありそうです。
