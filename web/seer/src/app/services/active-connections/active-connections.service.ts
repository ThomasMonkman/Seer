import { Injectable } from '@angular/core';
import { CookieService } from 'angular2-cookie/services/cookies.service';
import { Observable } from 'rxjs/Observable';
import { ReplaySubject } from 'rxjs/ReplaySubject';
import { ConnectionMeta } from 'app/classes/connnection-meta';

@Injectable()
export class ActiveConnectionsService {
  private readonly cookieKey: string = 'activeConnections';
  private connections: ConnectionMeta[];
  private readonly connectionsSubject: ReplaySubject<ConnectionMeta[]> = new ReplaySubject(1);
  constructor(private cookieService: CookieService) {
    const connectionCookie = this.cookieService.get(this.cookieKey);
    this.connections = connectionCookie.length > 0 ? JSON.parse(connectionCookie) : [];
    this.connectionsSubject.next(this.connections);
  }

  public get(): Observable<ConnectionMeta[]> {
    return this.connectionsSubject.share();
  }

  public store(connection: ConnectionMeta) {
    this.connections.push(connection);
    this.cookieService.put(this.cookieKey, JSON.stringify(this.connections));
    this.connectionsSubject.next(this.connections);
  }

  public remove(name: string) {
    this.connections = this.connections.filter(connection => connection.name === name);
    this.cookieService.put(this.cookieKey, JSON.stringify(this.connections));
    this.connectionsSubject.next(this.connections);
  }
}